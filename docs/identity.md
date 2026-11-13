# Identities

An identity is the player's name, skin and key in one portable file. It is the
same identity locally and online: the name and UUID in the world, the key in
peer-to-peer play. One identity can be online once at a time.

## The file

`<name>.lceid` is a ZIP archive (stored, not compressed, so any archive tool
opens it) with these entries:

| Entry | Holds |
| --- | --- |
| `identity.txt` | the manifest, UTF-8 `key=value` lines in a fixed order |
| `identity.sig` | an Ed25519 signature of the manifest by the identity's own key (64 bytes) |
| `identity.key` | the secret seed of the key (32 bytes) |
| `skin.png` | the player's own skin, only when the skin is bundled |

The manifest has `lce-identity=1`, `name`, `public-key`, `uuid`, and one of
`skin=<skin name>` (a skin the game already has, chosen by name) or
`skin-file=skin.png` with `skin-hash=<BLAKE2b-256 of the file>`. A file that does
not load unchanged is refused: a changed name or skin breaks the signature, a
skin that does not match its hash is refused, and the key must produce the
public key and the UUID that the manifest names. That is what makes an identity
traceable: the name and skin it shows are bound to the key.

The UUID is not stored by choice: it is the first 16 bytes of a BLAKE2b hash of
the public key under the label `lce identity uuid v1`, with the version set to 8
and the variant to RFC 4122. The same key always gives the same UUID, and a
renamed identity keeps it. The name is 3 to 16 letters, digits and underscores,
as the launcher's profiles require.

The file contains the secret key, so it is the identity: anyone who has the file
is that player. It is kept with owner-only permissions, and moving it to another
machine moves the player. It is not encrypted. Protecting it with a passphrase
is not done (`todo.md`).

## The folder

Identities live in one folder as `*.lceid` files. A file named `active` holds
the file name (without `.lceid`) of the identity in use. A file placed in the
folder is found the next time the folder is read; a file that does not verify is
listed as unusable and never used.

## Online once at a time

- On one machine, going online takes an exclusive lock on the identity
  (`<name>.lock`, held for as long as the process is online). A second copy of the
  game cannot go online with the same identity; it is told so. Closing the game,
  or crashing, frees the lock.
- Across machines, the rendezvous server refuses a second session for a key
  that already has a live one, and a game server refuses a login whose UUID is
  already on it. A session that stops sending is considered gone after 45
  seconds, so a crash does not lock the player out for long.

The same identity can still be used offline in any number of copies; the rule
is about being online.

## The screen

Help and Options has an Identities button under Change Skin (on the main menu, for
the primary player; not in a world). The screen (`UIScene_IdentityMenu`) uses the
skin screen's layout: a carousel of the identities that exist, each drawn with its
skin, the one in use ticked. Left and right turn the carousel. Below it are the
username, the friend code and the skin's source, and the identity's UUID is above.
The button list on the left acts on the identity in the centre: Use (not for the
one in use), Username (a keyboard; Y does the same), Skin (the usual skin screen,
for the identity in use only; a skin picked there is saved into the identity),
Skin file... (a system file window for a PNG, bundled into the identity), New
Identity (a keyboard for the username), Open identities folder (the system file
manager), and Remove (X, after a confirmation, never the one in use). While the
identity is online it cannot be changed or switched, and the screen says why.
An identity file that fails to load shows why under its name (its file name stands in
for the username). An unnamed entry would show "username..." in grey.

A skin chosen by name keeps the name; a bundled skin (a PNG inside the file) is
registered with the game's textures under a name made from its hash.

An identity that has no skin yet gets one of the 18 default skins from its name
(`Profile::DefaultSkin`: FNV-1a over the lower-cased name, a 32-bit finaliser, modulo 18, plus one; the one free constant, the hash's starting offset, was chosen so that the project owner's name, Anto248z, lands on Developer Steve (skin 18), and no name is listed anywhere), so
the same name always starts with the same skin. It is only the starting skin: the
first skin the player picks or bundles replaces it and is the one saved.

## Where it applies

The active identity gives the profile its name and UUID at sign-in, its skin is
applied by name (a bundled skin is registered with the game's textures first),
and its key is the peer-to-peer identity. Choosing and editing identities is a
screen (`UIScene_IdentityMenu`), laid out like the skin screen.

## Code

`source/identity` (library `lce_identity`, C++03): `Archive` (the ZIP),
`Profile` (manifest, signature, skin hash), `Uuid`, `Store` (the folder),
`Lock`, and `Files` (private writes, listing and removing, one file per
operating system).
