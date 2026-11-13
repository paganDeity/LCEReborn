# Peer-to-peer friends, lobbies and servers

Decision (project owner): a tiny rendezvous server does friend codes,
lobbies and NAT hole-punching, and relays only when punching fails. It is
built from the standalone server's code, self-hostable, and holds no game
state. Everything else is peer to peer.

Peer to peer is a transport, not a wire protocol. Identity, handshake, session,
rendezvous and relay carry the bytes of either wire protocol, Legacy or Reborn
(`compatibility.md`, On the wire); they do not define game packets and they do
not negotiate game features. That negotiation is Reborn's.

## Threat model

The rendezvous server and every peer are untrusted. The aim is that nobody
can:

- impersonate a friend or a host (identity is a key, not a name);
- read or alter a session (everything after the handshake is encrypted and
  authenticated);
- turn the rendezvous server or a host into a traffic amplifier (every
  unauthenticated reply is no larger than the request that caused it, and
  per-address rate limits apply);
- learn a player's address without being their friend or in their lobby
  (addresses are exchanged only between peers that both accepted).

Not goals: hiding that two players are playing, or defending a host from a
peer it chose to admit.

## Identity

Each install creates an Ed25519 key pair on first run. The public key is the
player's identity; the friend code is its fingerprint in a short, checksummed
form. A display name is a label the player chooses and carries no authority.
The private key stays on the device, in the platform's key store where there
is one.

## Handshake

Peers run an authenticated key exchange (X25519 for the shared secret, each
side signing the transcript with its identity key), then derive separate
send and receive keys for an AEAD (ChaCha20-Poly1305). A peer admitted to a
lobby is checked against the lobby's member list, which the host signs.
The session carries the wire protocol's bytes unchanged. Anything a wire
protocol negotiates happens above this transport, inside the encrypted channel.

## Lobbies and mini game servers

A lobby is a record signed by its host: owner key, mode, name, player count and
limit, visibility (friends, friends of friends, invite only), the wire protocol
it speaks and that protocol's version, so Join Game can mark lobbies it cannot
join. The server drops a lobby that is not refreshed for 30 seconds (the host's
heartbeat is publishing it again). The rendezvous
server stores records and hands out peer addresses only as above. A mini
game server is the same record with a dedicated host (the standalone
server), so a lobby can sit on a player's machine or on a server with no
change to clients.

## Implemented

`source/p2p` (library `lce_p2p`, test `lce_p2p_test`, run by `ctest`):

- `P2pIdentity`: Ed25519 identity from a 32-byte seed, signing, and the
  friend code (80 bits of a BLAKE2b fingerprint of the public key, written
  `XXXX-XXXX-XXXX-XXXX`; it binds the key, so a code names exactly one
  identity). The caller stores the seed.
- `P2pHandshake`: a three-message exchange with ephemeral X25519 keys. The
  responder and initiator each sign the running transcript with their
  identity key, both signatures travel encrypted, and an expected peer key
  can be required. The first message is padded to 192 bytes, never smaller
  than any reply, so an unauthenticated sender cannot use a peer to amplify
  traffic toward a spoofed address.
- `P2pSession`: XChaCha20-Poly1305 with a per-direction key and an explicit
  counter. A repeated or older counter is refused, and so is a message
  opened in the direction it was sent.
- `P2pRandom_<os>`: the operating system's random bytes (`_linux`,
  `_windows`; the Windows file is not compiled yet).

- `P2pLobby`: a lobby record signed by its owner (visibility, players,
  capabilities, port, name, mode); a reader rejects a changed record.
- `RendezvousServer` and `RendezvousClient`: the rendezvous protocol, free of
  any transport (datagrams in, datagrams out), so tests drive it in memory.
  Every datagram is `kind, 8-byte connection id, payload`; after the
  handshake each request and reply is sealed in the session.
  - `Register` sets the keys a peer allows (its friends).
  - `Connect` gives an address only when the target may be joined by the
    requester: with a lobby published, its visibility rule decides (friends:
    on the host's list; friends of friends: on the list of a friend of the
    host; invite only: invited by the host); with no lobby, the target must have
    allowed the requester. A refusal looks the same for "not allowed" and "not
    online", so presence is not revealed. On success both sides get the other's
    address at once, which is the simultaneous open of the hole punch.
  - `Publish` stores a host-signed lobby, which must belong to the sender;
    `List` returns the lobbies the requester may join, six per reply with a page
    offset; `Invite` adds a peer to an invite-only lobby and pushes the lobby to
    the invitee if it is online; `Unpublish` removes it.
  - `Lookup` turns a friend code into a public key (rate limited, no address);
    `Reflect` returns the sender's address as the server sees it.
  - `Relay` forwards sealed bytes only between a pair `Connect` authorised
    (two minutes, renewed by use), up to 1000 bytes each, with a byte budget per
    identity (32 KB a second, 64 KB burst) that survives a reconnect.
  - Limits: five handshakes per address per ten seconds, 4096 connections, a
    message budget per connection (bursts of 40, 20 a second, larger messages
    cost more), replayed datagrams ignored, idle connections dropped after 90
    seconds.
- `LCERebornP2pServer`: the UDP server (`--port`, default 25580, and `--key`,
  a 32-byte seed created with mode 0600 on first run). It prints the public key
  clients must pin and its friend code. It links the socket layer and the
  p2p library only.

`tests/rendezvous_test.cpp` covers permissions, lobby visibility, relay,
flooding, replay and expiry in memory; `lce_rendezvous_smoke` runs two
clients against a live server over UDP (`lce_rendezvous_smoke HOST:PORT
PUBLICKEY`).

Not yet: key storage on each platform, and the client side in the game.

## Decisions

- Wire protocols: Legacy only exists, so P2P carries Legacy bytes untouched.
  Reborn is separate and later; the transport does not change when it arrives.
- How a connection chooses Legacy or Reborn is an open question
  (`todo.md`, Reborn protocol). A lobby record names its protocol and that
  protocol's version, so Join Game can mark lobbies it cannot join.
- The early framing and negotiation code that touched the Legacy stream is
  removed (it extended Legacy); the `PacketCodec` seam stays.

## Connecting two peers

`P2pPeerLink` is one connection to one peer: the handshake (resent until it
completes, since datagrams are lost), the session, and the reliable stream
under it. The dialer sends its first message directly at once and, if nothing
completes within three seconds, through the relay as well; the host answers on
whichever path the message arrived. Both then send on the path of the latest
authenticated datagram, so a direct path that starts working later is taken.
The relay only ever carries the sealed datagrams. A host that does not admit
the peer sends a sealed reset and the dialer's link closes.

`P2pNode` is the client agent: it keeps a session with the rendezvous server
(registering friends every 15 seconds, which also keeps the NAT mapping open),
publishes the lobby on change and every 10 seconds, reflects its address, runs
the lobby list, and turns a punch into either a dial (the peer it asked for)
or probing (a peer that asked for it; it sends probes to the other side's
address for 15 seconds so its NAT lets the first handshake in). It creates a
link for an incoming handshake only from an address it was told to expect, or
from a peer key it was told to expect through the relay, so an unsolicited
sender cannot make it allocate anything. `P2pNode` does no I/O: datagrams go in
and come out, and the platform layer owns the socket.

## In the game

The Legacy connection layer runs unchanged over P2P. `Link` reads and writes a
`Stream` (`net/NetStream.hpp`); a TCP socket is one, and a peer link is another.
`Link::Connect(text)` asks the `Dialer` registered for the text's scheme, so
`p2p:<key>` dials a peer and anything else is a TCP address. `NetP2p.hpp` is the
service interface; the PC backend (`platform/pc/PcP2pService.cpp`) owns the UDP
socket, a thread that pumps `P2pNode`, the active identity's key (`identities.md`) and the friends (`p2p-friends.txt`); the Wii U backend is inert.
Time, random bytes and private files each have a `_linux` and `_windows` file
under `source/p2p`.

`CPlatformNetworkManagerLan` uses it in three places and nothing else changed:
`UpdateBeacon` publishes the hosted game (friends only, or friends of friends
when the game is public) and withdraws it when hosting stops; the accept path
adopts accepted peer streams exactly as it adopts accepted sockets; and the
session list shows the lobbies the server returns, marked outdated when the
lobby's protocol or version differs from this build's. A host admits a peer if it
is a friend, was invited, or the lobby is open to friends of friends (the server
has already checked that rule).

A dedicated server hosts the same way: `LCERebornServer` takes the same three
`--p2p-*` options, publishes its world once it is ready, and admits the friends
listed in its `p2p-friends.txt` (it has no screen to add one). A guest cannot
tell it from a player's game.

## Screens

Existing screens keep their look and flow; peer to peer shows up in them:

- Join Game lists P2P lobbies beside LAN games and saved servers, and marks
  one outdated when its protocol or version differs. Selecting one opens the
  usual join screen.
- The host options that exist already map onto the lobby: Invite Only publishes
  an invite-only lobby, Allow Friends of Friends a friends-of-friends lobby, and
  otherwise friends only.
- Players/Invite lists the players and kicks them as before. While hosting
  online, Y opens the Friends screen to invite someone.

What is new is one screen, Friends (`UIScene_FriendsMenu`), built on the Players
screen's layout: your own friend code on top, the Add Friend button, and your
friends (nickname and code). On a controller: A invites the selected friend
while hosting online, X removes them (after a confirmation), Y changes the
nickname, B goes back. Add Friend and the nickname change reuse the Edit Server
screen in a friend mode (nickname and friend code; the code is fixed once a
friend is added). Join Game has a Friends row that opens it, when peer to peer
is on. Mouse and keyboard work as on every other screen, and a text field opens
the on-screen or system keyboard as Edit Server does. Strings come from
`IDS_LCE_*` in `ExtraStrings.hpp` and `L10N.cpp`.

A code the server does not know comes back as a denial; the Friends screen then shows
"No one has that friend code" (`Node::TakeLookupFailure`; one message per failed
lookup, checked in the node test, not seen on screen).

Not observed: the layout on a controller or in split screen, the wording in
languages other than English (the new strings only have English text), and the
confirmation and error dialogs, which I opened only far enough to check they do
not crash. A person has to check these; the list is in `todo.md`.

## Wire formats

Every P2P message has explicit field widths and byte order, built and read
through `Writer` and `Reader`, never by casting a struct or reading an
unaligned integer. All fields are big-endian except the session counter and
the nonce derived from it, which are little-endian (8 bytes, least significant
byte first). The rendezvous messages are built in one place,
`RendezvousMessages`, used by both client and server. `lce_p2p_test` encodes
every message again with a reference encoder that works from host integers,
once as the host holds them and once as a host of the opposite byte order
would, and requires all three to match byte for byte, then decodes each one
back to its fields.

## What the Legacy connection layer needs

Measured in `source/net/NetLink.cpp`: a `Link` reads length-prefixed frames
with `ReadExact`, writes them with a blocking `SendAll`, polls `Receive` with a
100 ms timeout and tells data (more than zero), timeout (zero) and close
(negative) apart, and shuts the socket down to stop. The TCP socket sets
`TCP_NODELAY`, so a small write must leave at once. It never needs datagram
boundaries. So the contract under Session is a reliable, ordered byte stream
with back-pressure, and nothing else.

`P2pReliableStream` provides it over lossy datagrams: a 128-packet window of
900-byte payloads, cumulative acknowledgements with a 32-packet selective
bitmap, a round-trip estimate that sets the retransmit timer (60 ms to 2 s,
backing off), a close that arrives after the data, and failure after 14
retries. The test moves 100 KB to 300 KB through links with up to 25% loss,
10 to 20% duplication and 30% reordering, byte for byte.

## Order of work

1. Identity and the handshake as a library with a test, no network. Done.
2. The rendezvous server: friend codes, lobby records, hole-punching. Done,
   except that the peers' own handshake after the punch is not wired yet.
3. Relay for peers that cannot connect directly.
4. Client: friends list and lobby screens (built on the shared menu layer).
5. Mini game lobbies and servers on top.

Crypto comes from a small audited library (Monocypher is the candidate:
one source file, public domain); it is vendored, not written here.
