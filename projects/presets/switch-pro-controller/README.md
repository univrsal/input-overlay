# README

## Mapping Switch Pro Controller inputs

To use this preset, you must map your Nintendo Switch Pro controller inputs to those of an Xbox 360 controller.
The preset expects following mapping from the Pro controller to corresponding Xbox 360 buttons:

* Home → Guide
* Minus → Back
* Plus → Start
* R → RB
* L → LB
* ZR → RT
* ZL → LT
* X → Y
* Y → X
* A → B
* B → A

DPad, left and right sticks should be mapped one-to-one directly.

To enable this mapping, first install [ViGEmBus Driver](https://vigem.org/projects/ViGEm/How-to-Install/) which allows both following solutions to emulate an Xbox 360 controller.

If you plan on using your Pro controller to play PC games, you may connect your controller to your PC using bluetooth connexion and use [X360ce](https://www.x360ce.com/) to map the buttons accordingly.

If you want to play games on your Nintendo Switch and show your inputs in a stream, a solution is to acquire a [Titan Two](https://www.consoletuner.com/products/titan-two/) device.
This piece of hardware will receive inputs from your controller and dispatch them with hardly no latency between your computer and your console through following connexions:

* Connect the Titan Two and PC through USB on `PROG` port
* Connect the Titan Two and Nintendo Switch through USB on `OUTPUT` port
* Connect the Titan Two and your controller either through USB on `INPUT A` port, or wirelessly if your Titan Two comes equipped with a bluetooth module

Then, in [GTuner IV](https://www.consoletuner.com/software/gtuner-iv/), enable `Show Virtual Controller configuration area on Device Monitor` in menu `Tools > Preferences > Devices`.
In the `Device Monitor` panel, set the `Virtual Controller` to `INPUT → Xbox 360 controller` and finally click `RUN`.

## Changing the controller appareance

You may edit freely the `switch-pro-controller.svg` template with any vector graphics editor like [Inkscape](https://inkscape.org/).
Just be careful not to move any texture and not to increase the size of the bounding box of each button.

When you are done with your modifications, export the image in PNG with transparency enabled and a resolution of 1280x1400 (96dpi).
