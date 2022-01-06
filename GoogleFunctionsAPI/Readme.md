### These two files (requirements.txt and main.py) are placed to Google Functions API

Don't forget to update the personal API key and other parameters (destination and location place IDs) in main.py

Set the **TRIGGER** > Trigger Type > HTTP in Google Functions API (Current Wemos sketch does not work with HTTPS)

Set **Runtime, build, connections and security settings**  > Connections > Allow all traffic

Set **Permissions** > Cloud Function invokder > Allusers


Tip: Set trigger alarms for Google Cloud Budget for being informed of any exceedence of limits before being charged by surprise.

