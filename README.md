# "Gaming" Chair

So we decided to build a gaming chair... and by that we mean a chair that you can use to play video games!

[Back of the chair, set up to play Crazy Taxi](chairpicback.jpg) [Front of the chair; we will definitely be installing wheels, the cart was NOT the play](chairpicfront.jpg)

Below you can find a quick writeup on the components of our chair, as well as the BOM in case you want to build a "gaming" chair yourself or adapt an existing chair into one.

## Woods

Our chair is 29.5" x 23.5" x 57"; it's more of a gaming THRONE than a chair now that I think about it.

For the legs, we fashioned 7 pieces of about 2.5" x 2" x 24" and cut down 3 of them, domino jointing and woodgluing them horizontally to the other 4 legs for support (we left the front open so a wood support piece wouldn't interfere with dangling legs).

For the seat (22" x 21.5" x 1") and the back (24" x 1" x 32") we lucked out and happened to find pieces that mostly fit; we just had to do a bit of woodgluing and planing.

For the arms, the goal was to make triangle supports and domino joint/glue them to the back so that the triangle joints could be glued down to the seat for extra support; we mismeasured and so we had to make extension pieces to the side of the seat to provide extra support for the arms.

For placing the electronics, we drilled holes for the wires, chiseled out a place for the joystick (left arm) and buttons (right arm) and hand-routed into the back of the chair for a place for all the electronics.

## 3D Designs

There are currently 3 primary 3D designs that we used:

- The start lever
- [Left and right triggers](resources/models/TriggerButtons.f3d)
- [The foot pedals](resources/models/FootPedal.f3d)

Those designs can be found in the `resources/models` directory.

We also tried to make a box for cable management, but it didn't fit on the build plate of any of the printers that we had access to and so we ended up with a chair that 29 people (and counting) have said looks like an electric chair.

## Fabrics

We started to do upholstery, but ultimately decided to scrap it before the Purdue Hackers showcase due to time constraints; instead, we wrapped our foam and sewed in velcro, as well as stapling velcro to the back and the seat so the foam cushioning could be attached.

## Electrical

We bought a bunch of braided ribbon cable for the project; while this was good in terms of flexibility, it made soldering and wiring a bit of a nightmare. I've lost count of the number of solder joints that have broken at this point (mostly due to transport). We used an ESP32 microcontroller for the controller itself, analog hall effect sensors rigged with magnets that move closer to the sensor for the triggers and footpedals, and just attached the wires opposite each other for the start lever so they contact on pulling the lever. We battery-powered all the sensors which is probably unnecessary (you can probably get away with using 3v3V and stepping it up to 5V using a booster), but it's kinda funny so we kept it.

In terms of the electrical components we ended up using:

- Arcade buttons: https://a.co/d/8YRFZ9m
- Analog hall effect sensors: https://a.co/d/5u7OO93
- ESP32 microcontroller: https://a.co/d/g4PPseC
- Analog joystick: https://www.adafruit.com/product/3102
- Miniboost: https://www.adafruit.com/product/4654
- 6x Magnets: https://www.adafruit.com/product/9

## Programming

We were initially going to be using Bluetooth, but realized that there wasn't any point with all the wires we had anyway and so we migrated to communicating over USB serial (src/main.cpp) with a python script (src/controller.py) that uses the vgamepad library to emulate an xbox360 controller.

## Next Steps

We have commitments from people to play Dark Souls, Celeste, Cuphead, Silksong and various other games with the chair, so we want to make sure it's a polished gameplay experience. We plan on reworking the pedals, since the current solution has too much spring tension and is awkward to rest your feet on, as well as cleaning up some of the electrical and either upholstering or spray painting the chair (some kind of design work).

## Contributions

Thanks to @Benjamin for his help with the woodworking and electronics, to @Soham for his help with the electronics and 3D design and to @Grace for her help with soldering. Huge thank you to @PurdueHackers for funding this project and for organizing an awesome event for us to show it off and to @BechtelInnovationDesignCenter for helping us with woods and electronics and allowing us to borrow the tools needed to make this project a reality.
