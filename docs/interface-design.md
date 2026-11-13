# Interface design for identities, friends, chat and voice

A proposal for the owner, written before any of these screens change (plan Phase H, decisions D7 to
D11). Each section says what exists, what the retail Legacy interface does in the same place, and what
I recommend, with a layout drawn from a scene the game already has. Nothing here is built. Where a
recommendation adds something retail never had, it says so; that is a disclosed divergence, not a
fidelity claim.

The rule behind every choice: a feature the console never had goes where the console put the nearest
thing it did have, uses that screen's layout and controller buttons, and adds no new kind of screen.
A player who knows the Wii U edition should find each feature by looking where they would have
looked.

## D7. Identities

Today: Help and Options, under Change Skin, opens `UIScene_IdentityMenu`, which reuses the skin
screen's carousel (`identity.md`). The main menu shows nothing about who is playing.

Retail: the console shows the signed-in profile's gamertag at the top of the main menu, and switching
profile is a system action, outside the game.

Recommendation:

- Keep the screen where it is. Help and Options is where the console keeps everything about the
  player (skin, controls, settings), and an identity is the PC's stand-in for a console profile.
- Show the active identity's name where retail shows the gamertag, top right of the main menu, and
  make it the one shortcut: on PC, clicking it, or Y on a controller while the main menu has focus,
  opens the Identities screen. Retail's main menu has no Y action, so nothing is displaced.
- A passphrase for the identity file is asked for once per run, the first time the identity goes
  online, in the game's keyboard dialog; never at start, so offline play is never blocked.

```
            MINECRAFT                         Anto248z  [Y]
                                              -------------
             [ Play Game        ]
             [ Mini Games       ]
             [ Leaderboards     ]
             [ Achievements     ]
             [ Help & Options   ]   -> Change Skin / Identities / ...
             [ Packs            ]
             [ Exit Game        ]
```

## D8. Friends and presence

Today: `UIScene_FriendsMenu`, built on the Players screen's layout, reached from a Friends row in
Join Game and from Players/Invite while hosting (`p2p.md`, "Screens").

Retail: friends come from the console's friend list; Join Game lists the friends' joinable games, and
the in-game Players screen invites.

Recommendation: exactly two doors, both where retail had friends:

1. Join Game: a friend whose game can be joined is listed as a game, as retail lists a friend's game.
   The Friends row stays at the top of the list as the place to add and manage friends.
2. In a world, Players/Invite: Y opens Friends to invite, as now.

Presence is only "playing a world you can join" (the retail joinable marker beside the name), "online"
or nothing. No status text, no last-seen time: retail has neither, and each is a privacy cost
(`p2p.md`, threat model).

```
  Join Game
  [ Friends: add or manage            ]
  [ Anto248z's World        (friend)  ]   <- joinable, marked as retail marks a friend's game
  [ LAN: Kitchen PC                   ]
  [ Saved server: example.org         ]
```

## D9. Chat and moderation

Today: chat is the console's own (`ChatScreen`): the prompt bar, the fading lines above the hotbar,
the controller panel with Quick Chat and the on-screen keyboard. The keyboard field along the bottom,
history and completion are PC additions (`pc-platform.md`, "Chat").

Recommendation:

- Keep the console's opening (the prompt bar, D-pad Right, Y in the pause menu) and its tooltips. The
  Java-style text field on PC stays as the keyboard's behaviour; it is a disclosed divergence because
  retail never had a keyboard path.
- Mute and block live in the Players screen, on the player's row, beside kick: the place retail uses
  for actions on another player. Mute hides that player's chat and voice for you; block also keeps
  them out of games you host. Both are kept per identity (by UUID and key), so they survive a rename.
- The host's kick and mute are the same row's actions with host privileges, as kick is today.

```
  Players                      [A] Invite  [X] Kick  [Y] Friends
  Anto248z (host)     (speaker)
  Guest          [muted]
  > Player2      (speaker)     [LB] Mute   [RB] Block
```

## D10. Voice chat

Nothing exists. Retail had voice through the console; the game showed a speaker icon beside a talking
player in the Players screen and the in-game player list.

Recommendation, scope first:

- Push to talk by default (a key and a controller button, both rebindable in Controls), with open
  microphone as a setting. Proximity is a host option, off by default, as it changes how a game
  plays.
- Settings go in Audio, after the existing sliders: input device, output device, voice volume,
  microphone volume, push to talk or open. Nothing in a new screen.
- The speaker icon appears where retail draws it: Players screen row and the in-game player list.
- Voice travels on the Reborn connection (a reserved channel, `reborn-protocol.md`), so a Legacy
  connection has no voice, and voice never reveals a peer's address: it goes through the host, never
  peer to peer between guests. No voice from a peer that is not authenticated.
- The dedicated server relays voice like the hosted game; it has no microphone of its own.

```
  Audio
  Music            [=======   ]
  Sound            [========  ]
  Voice volume     [======    ]
  Microphone       [=====     ]
  Voice            < Push to talk >
  Input device     < Default >
  Output device    < Default >
```

## D11. Bedrock interface slices

Order proposed, each built only after the shared menu model (plan G1):

1. Touch layout from `touch-layout.json` and its editor (needed by Android).
2. HUD, safe areas, portrait and landscape.
3. Inventory, crafting and creative, which a touch player cannot play without.
4. Containers, then chat and the command keyboard.
5. Skins, world list, achievements and leaderboards, prompts and tutorials.

## What I need from the owner

One answer per section: keep the recommendation, change it, or drop the feature. Until then none of
these screens change.
