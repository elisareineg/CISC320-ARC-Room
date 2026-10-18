# Adding Your Room

How to add the Gym, the Cafeteria, or anything else to the game. Get the game running first (see
the README) and play the Library once so you know how everything works.

One thing about names: in the code, each place you pick from the menu (Library, Gym, Cafeteria) is
a **map**. A map can have several **rooms**, which are the screens inside it. The Library map has
three rooms, one for each floor.

## What you touch

You only make new files in `src/maps/`, put your pictures in `assets/yourmap/`, and add one line to
`src/maps/MapList.cpp`. Everything else in `src/` is shared, so if you think it needs to change,
ask the group first since it changes every map.

`src/maps/StaufferMap.cpp` (the Library) is the example to copy from.

## How a map works

Your map is a class that inherits from `Map`. The game calls these functions on it:

- `getName()` returns the name shown in the menu
- `setup()` runs once at the start. Make your rooms and hotspots here, set the timer, and say the
  intro
- `onClick(id)` runs when the player clicks a hotspot
- `onUseItem(item, id)` runs when the player picks an item and then clicks a hotspot
- `onKeypadSolved(keypadId)` runs when someone types the right code into a keypad
- `getHint()` returns the hint for when the player presses H

The game does all the drawing, the inventory, the timer, the keypad, the Dean, the Home button,
stars and saving to the player's profile. You never draw anything yourself. Stars are worked out in
`calculateStars()` in `Profile.cpp` (3 stars for 10 minutes or less, 2 for 20, 1 for finishing).

A **hotspot** is a box in a room the player can click. It has an id that your code checks (like
`"whiteboard"`) and a label the player sees (like `"Whiteboard"`).

A **flag** is how your map remembers something happened. `setFlag("power on")` saves it, and
`hasFlag("power on")` checks it later.

## Making your map

1. Copy `StaufferMap.h` and `StaufferMap.cpp` in `src/maps/` and rename them, for example
   `GymMap.h` and `GymMap.cpp`.
2. In both files, change `StaufferMap` to `GymMap`, including the `#include` at the top of the .cpp.
3. Add both files to your Visual Studio project.
4. In `MapList.cpp`, add `#include "GymMap.h"` at the top and add a case:
   ```cpp
   case 1:
       return new GymMap();
   ```
5. Run it. Your map shows up in the menu but still has the Library inside.
6. Change `getName()`, delete the Library's rooms, hotspots and `if` statements, and write yours.

## How the Library does it

`setup()` makes each room with `addRoom`, then its hotspots, then its hiding spot:

```cpp
addRoom("floor3", "Floor 3: Silent Study", "stauffer/floor3.png");
addHotspot("floor3", "whiteboard", "Whiteboard", 350, 100, 320, 180);
addHotspot("floor3", "carrel", "Study carrel", 80, 330, 330, 150);
setHidingSpot("carrel");
```

`onClick()` is one long `if / else if` with a branch for each hotspot. A lot of them check a flag
first so the hotspot does something different once it's been used:

```cpp
} else if (id == "stairs3") {
    if (hasFlag("stairs3 open")) goToRoom("floor2");
    else openKeypad("stairs3", "3719");
}
```

`onUseItem()` has a branch for each item and hotspot pair that does something, and an `else` at the
end for everything else:

```cpp
if (item == "Flashlight" && id == "whiteboard") {
    setFlag("whiteboard read");
    say("In the flashlight beam the faded marker shows up: 3 _ 1 _");
} else if (item == "Toonie" && id == "vending") {
    ...
} else {
    say("That doesn't do anything.");
}
```

`onKeypadSolved()` sets the flag that `onClick()` checks. `getHint()` looks at which room you're
in, then goes through the flags in order and returns the first thing you haven't done.

Some things you can look up in `StaufferMap.cpp`:
- picking something up only once: `lostfound`
- an item that gets used up: the toonie on `vending`
- a door with a code: `stairs3`
- something that only works after something else: `terminal` needs `"power on"`
- a code split into two clues: the whiteboard and the bookshelf
- winning: the keycard on `doors`

## Helper functions

| Function | What it does |
|---|---|
| `setTimeLimit(600)` | time limit in seconds |
| `addDeanVisit(300)` | the Dean comes when this many seconds are left, once per call |
| `addRoom(id, title, picture)` | adds a room, the first one is where the player starts |
| `addHotspot(room, id, label, x, y, w, h)` | adds a clickable box to a room |
| `setHidingSpot(id)` | where to hide from the Dean in that room |
| `goToRoom(id)` | moves the player to another room |
| `say("text")` | shows a message, call it more than once for more messages |
| `giveItem("Name")` / `removeItem("Name")` / `hasItem("Name")` | inventory |
| `setFlag("name")` / `hasFlag("name")` | remember that something happened |
| `openKeypad(keypadId, "1234")` | opens the keypad, the code can be any length |
| `win()` | the player escaped |

`currentRoom` is the id of the room the player is in.

## Getting the visuals

The game is drawn with SFML, which can show pictures, shapes and text. Each room in your map is
**one background picture**, and the hotspots are invisible boxes placed on top of it. So getting
the visuals for your map just means getting one picture for each room. Items in the inventory are
text boxes, so they don't need pictures.

You don't need pictures to start. Without one, every hotspot is drawn as a grey box with its label,
so build and test the whole map first and add pictures at the end.

Ways to get the pictures:

- **Take photos on campus.** The easiest and it looks the most real. Hold the phone sideways, stand
  where the player would be standing, and get everything you plan to make clickable in the shot.
- **Draw them** in any drawing program and export a PNG.
- **Use free photos** from Wikimedia Commons. These need credit, so add the photo's author and
  licence to `assets/CREDITS.txt` (the menu backgrounds are done this way).

To make the dark, creepy look, turn down the brightness and add a red or blue tint in any photo
editor (the Windows Photos app works).

Then:

1. Crop the picture to a wide shape (16:9) and resize it to **1280 x 720**. Paint can do both.
2. Save it as a PNG in your own folder in `assets/`, using the path you gave `addRoom`. For
   `addRoom("court", "Gym: Court", "gym/court.png")` the file goes in `assets/gym/court.png`.
3. Run the game. The picture is stretched over the room area, which is everything between the top
   bar (y = 40) and the inventory (y = 640), so keep the important stuff in that part.
4. Press **D**. It outlines your hotspots and shows the mouse x, y next to the cursor. Hover over
   the top left corner of something in your picture (a door, a desk), then the bottom right, and
   use those numbers in `addHotspot(room, id, label, x, y, width, height)`.

## Before you push

- Play your map all the way through without hints, and once where the Dean catches you.
- If something is wrong in `setup()`, a "Setup problem" message shows up when your map starts. It
  catches a hotspot in a room that doesn't exist, two hotspots with the same id, and a room with no
  hiding spot.
- Hotspot ids can't repeat anywhere in your map, even in different rooms (`door1`, `door2`).
- Item names have to match exactly. `"Small key"` and `"Small Key"` are different, so copy and
  paste them.
- Only add your own files when you commit:
  ```
  git add src/maps/GymMap.h src/maps/GymMap.cpp src/maps/MapList.cpp assets/gym
  ```
  Don't use `git add .`, it picks up Visual Studio's build files too.

## If something goes wrong

- Lots of errors inside the SFML files: the project is on C++14, change it to C++17.
- `unresolved external symbol` (LNK2019): a .cpp file isn't added to the project.
- `did not override any base class methods`: a function name or its parameters don't match
  `Map.h`.
- Your map isn't in the menu: the case is missing in `MapList.cpp`, or the case numbers skip one.
- A hotspot doesn't show up: the room id is spelled wrong, or it's outside y = 40 to 640.
- Using an item does nothing: the item name or the hotspot id doesn't match exactly.
- No pictures at all, not even the menu: the game can't find `assets/`, check the working
  directory.
