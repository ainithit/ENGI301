<h1>HIFI Music Player<h1>
<h2>Getting started with the BELA Mini<h2>
<h3>1. Flashing the micro SD card with the latest Bela software (https://learn.bela.io/using-bela/bela-techniques/managing-your-sd-card/)<h3>
<h4>a. Download and install Balena Etcher, an open-source software program used for copying image files into stoage media.<h4>
<h4>b. Download the latest Bela software image from the Bela Github Page (https://github.com/BelaPlatform/bela-image-builder/releases/).<h4>
<h4>c. Insert the micro SD card into the computer and flash the image by following instructions displayed on Balena Etcher.<h4>
<h4>d. Insert the micro SD card into the BELA system.<h4>
<h3>2. Plug the BELA Mini into your computer with a micro-USB to USB cable.<h3>
<h4>a. The SD card should boot properly given that the micro SD card was flashed correctly.<h4>
<h3>3. Load the BELA IDE by opening a web browser (chrome is recommended) and going to http://bela.local<h3>
<h3>4. There you go! You're ready to begin programming on the BELA Mini!<h3>
<h1> <h1>
<h2>Coding with Python on the BELA Mini<h2>
<h3>DISCLAIMER: I was unable to code on Python on my BELA Mini due to many different complications but the main idea should be following the steps below. If you run into trouble downloading python, try contacting BELA support. 
<h3>1. Connect the BELA Mini to the internet (https://learn.bela.io/using-bela/bela-techniques/connecting-to-wifi/)<h3>
<h4>a. Connect the wifi adapter to the BELA Mini system.<h4>
<h4>b. From the terminal on your computer, ssh into the BELA system by typing in ssh root@bela.local<h4>
<h4>c. You should see the prompt "root@bela:~# lsusb" if your BELA has been connected.<h4>
<h4>d. Get the name of your wifi device by running the command "ip a." The name of the device should be wlanX, where X is a number.<h4>
<h4>e. Next, we will edit the interface list by running the command line "nano /etc/network/interfaces." Follow the comments within the file to edit the interface list. Press CTRL+X to exit and Y to save changes.<h4>
<h4>f. Run the following command, replacing YourNetworkName with the name of your Wifi network and YourNetworkPassword with the Wifi network’s password: wpa_passphrase YourNetworkName YourNetworkPassword >>/etc/wpa_supplicant/wpa_supplicant.conf <h4>
<h4>g. Run the following command to reset your device: ifdown wlan0; ifup wlan0;<h4>
<h3>2. Updating packages (modify etc/apt/sources/list) → change deb.debian.org to archive.debian.org<h3>
<h3>3. Install python3-pip → apt-get install python3-pip<h3>
<h3>4. Updating python version to from 3.5.3 to 3.9 (pybela need greater than 3.9).<h3>
<h4>a. Follow steps found here: https://gist.github.com/dev-sareno/1d2bf9cad3a0dc281f0bb16d501a4388<h4>
<h3>5. Download pybela → python3.8 -m pip install pybela<h3>




