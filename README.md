# Tramtime 🚋
![Tramtime](https://user-images.githubusercontent.com/97253116/148444243-2cc14ea0-9393-4980-9913-b5c71a78c1a7.png)


### IoT for displaying public transport times


## What it does 

Displays the time and line number of the coming next two public transport at a certain stop and going to a certain destination by using Google Directions API and Google Cloud Functions API. 

## Main Components

Wemos d1 mini

E-ink screen

18650 battery

TP4056 charger

Step up converter

3D printed enclosure


## Why Google Directions API

I originally wanted to use a local public transport API (9292.nl) but they turned out to be not free. So I signed up for Google Directions API which gives you a free credit and free request (under limit) every month. 

## Why Google Functions API

Google Directions API delivers back a json file. It is really long and I cannot parse it with Wemos d1 mini (insufficient RAM). Because I did not have a better dev board with higher RAM (e.g. ESP32) at the moment of developing this project, I went through with Google Functions API to parse this data. 

## What is included in this repository

Wemos d1 mini sketch (tested in Arduino IDE) 

Python code for Google Functions API

STEP files for the enclosure (3D print) 

## Summary of the functions

### Hardware
E-ink screen => Wemos d1 mini > step-up converter > TP4056 charger > 18650 battery

USB micro port for charging the battery

Reset button for Wemos d1 mini

### Software

* Wemos d1 mini connects to a prescribed WIFI
* Sends a request to Google Functions API (GFAPI)
* GFAPI gets the next two tram times and line numbers from Google Directions API (GDAPI)
* GFAPI returns the retrieved info back to Wemos d1 mini
* Wemos d1 mini parses this info and prints it to the E-ink display
* Wemos d1 mini goes to deep sleep and wakes up 5 minutes before the next tram time to check the times again for a fresh update in case of any delay
* Wemos d1 wakes up and this loop restarts


Foot note: I know it is overengineered solution but until I learn better. 
