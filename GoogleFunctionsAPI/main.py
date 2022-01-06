from datetime import datetime   
from urllib.request import urlopen
import json 
import pytz

def hello_world(request):
    request_json = request.get_json()
    """if request.args and 'message' in request.args:
        return request.args.get('message')
    elif request_json and 'message' in request_json:
        return request_json['message']
    else:"""

    next_tram1 = next_tram()

    #----delay between two query (tram1 tram2)
    timeout = (datetime.now().timestamp()) + 0.2   # .2 s delay. If missing Google API returns error
        #print(timeout)
    while (datetime.now().timestamp() < timeout):
        True

    #----departure time query(depart at) for second tram
    #assuming second tram will arrive after first tram: requests 60 s later from the tram1 departure  to find the second tram depart time
    # if "no data" case for tram1, then add 60 s delay directly.
    if (step_num_transit == None):
        delay1=60
    else:
        delay1 = int( int(time1_timestamp) - int(datetime.now().timestamp()) + 60)
    #print (delay1)

    #----second tram query
    second_tram =  next_tram(delay1)

    #----if response of tram2 is the same as tram1, then add 60 s more to the delay
    if (next_tram1 == second_tram):
        delay1 = delay1 + 60
        second_tram =  next_tram(delay1)


    if (step_num_transit == None):
        seconds_to_second_tram = 1200 # if no data is found, set it to 1200 s  so device wakes up in 20 min 
    else:
        seconds_to_second_tram=time1_timestamp - int(datetime.now().timestamp()) # time1_timestamp is of the last processed tram request (2. tram).
    #so it subtracts the seconds of now from the last tram departure time to find the difference = how many seconds to go till the 2nd tram
    return (f"Now:{timenow()},{next_tram1},{second_tram},{seconds_to_second_tram}")


def timenow():
    tz = pytz.timezone('Europe/Amsterdam')
    now = datetime.now(tz).strftime("%I:%M %p")
    return (now) 


def next_tram(delay=0):
    global str_next_tram , time1_timestamp, step_num_transit
    step_num_transit = 0
    if (step_num_transit == None): return("no data 2nd")    
    
    # you can find PLACE_ID info of destination and location from here: https://developers.google.com/maps/documentation/places/web-service/place-id
    
    urlpart1 = "https://maps.googleapis.com/maps/api/directions/json?origin=place_id:ChIJv0WNzCu3xUcRaAqObvLtw0A"
    urlpart2 = "&destination=place_id:ChIJv6K4Yhe3xUcRxgdRxELZjvc&mode=transit&transit_routing_preference=less_walking"
    urlpart3 = "&departure_time="
    urlpart4 = str(int(datetime.now().timestamp()) + delay)
    #print(urlpart4)
    APIkey ="&key=YOUR_GOOGLE_DIRECTIONS_API_KEY_HERE"
    url = urlpart1 + urlpart2 + urlpart3 + urlpart4 + APIkey
    #print(url)
    response = urlopen(url)
    data_json = json.loads(response.read())
    #print(data_json)

    #find the correct step num of TRANSIT 
    #length of steps list
    steps_len = len(data_json.get("routes")[0].get("legs")[0].get("steps"))

    #find transit_mode=TRANSIT in step list. assign the item number to step_num_transit
    step_num_transit = None
    for x in range(steps_len-1):
        if (data_json.get("routes")[0].get("legs")[0].get("steps")[x].get("travel_mode") == "TRANSIT"):
            step_num_transit = x 
    #if TRANSIT could not found; give error, leave def()
    if (step_num_transit == None):
        return("No_tram  : ")

        
    time1 = (data_json.get("routes")[0].get("legs")[0].get("steps")[step_num_transit].get("transit_details").get("departure_time").get("text"))

    #print(time1)
    time1_timestamp= (data_json.get("routes")[0].get("legs")[0].get("steps")[step_num_transit].get("transit_details").get("departure_time").get("value"))
    tramline1 = (data_json.get("routes")[0].get("legs")[0].get("steps")[step_num_transit].get("transit_details").get("line").get("short_name"))
    str_next_tram = str(tramline1)+ str(" ")+ str(time1)

    return(str_next_tram)

