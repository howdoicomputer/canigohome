/*
* The entirety of this program revolves around creating a webhook that performs
* a GET request against the Bing Maps API that returns the duration traveled,
* in seconds, for the route between my work and my home. It then takes said
* duration and uses that to determine whether or not to turn on an LED light as
* an indicator that my commute will take less than fifteen minutes.
*
* The polling function is on a five minute interval with a fifteen second
* wait time to give wiggle room for the API call. During those five minute intervals,
* the wifi chip on the Photon is shut down to save power. This is so that I can
* squeeze more out of the power bank that I use to make the setup portable.
*
* Bing Maps Request URL:
*
* http://dev.virtualearth.net/REST/V1/Routes/Driving?wp.0=4655%20Great%20America%20Pkwy,%20Santa%20Clara&wp.1=360%20S%20Market%20Street,%20San%20Jose
*
* There is a "capture template" defined for the incoming data that parses
* the returned JSON so that only the duration is returned.
*
* Template: {{#resourceSets}}{{#resources}}{{travelDurationTraffic}}{{/resources}}{{/resourceSets}}
*
* Particle Webhook Resources:
*
* https://docs.particle.io/guide/tools-and-features/webhooks/
* https://community.particle.io/t/tutorial-webhooks-and-responses-with-parsing-json-mustache-tokens/14612/7
* https://github.com/rickkas7/particle-webhooks
*
* Bing Route API Documentation:
*
* https://msdn.microsoft.com/en-us/library/gg636957.aspx
* https://msdn.microsoft.com/en-us/library/ff701705.aspx
*/

/*
* A debug function thaat allows me to print numeric values with a message
* to the particle.io logs page.
*/
void debug(String message, int value) {
    char msg [50];
    sprintf(msg, message.c_str(), value);
    Particle.publish("DEBUG", msg);
}

/*
* Declare the led that we want to send output to.
*
* NOTE: This is the onboard LED for the Photon.
*/
int led = D7;

/*
* This function handles the incoming response from our created webhook.
*
* If the duration value is greater than fifteen minutes then turn on the onboard
* led. Else, turn the led off.
*/
void getTravelTimeHandler(const char *event, const char *data) {
    unsigned int durationInSeconds = atoi(data);
    unsigned int durationInMinutes = (durationInSeconds / 60);
    
    if (durationInMinutes < 15) {
        digitalWrite(led, HIGH);
    } else {
        digitalWrite(led, LOW);
    }
    
    debug("It would take %d minutes to get home.", durationInMinutes);
}

/*
* Subscribe to the get_travel_time webhook and set the led pin mode.
*/
void setup() {
    pinMode(led, OUTPUT);
    Particle.subscribe("hook-response/get_travel_time", getTravelTimeHandler , MY_DEVICES);
}

/*
* At the start of the loop, connect to particle.io's cloud platform so that we can trigger our webhook. Note,
* this is required because I put the Broadcom chip to sleep to save power at the end of the loop.
*
* Trigger the get_travel_time webhook so that it makes that GET request against the Bing Maps API. The
* delay is there so that the response from the webhook has time to trigger its handler function before
* the System.sleep function is called to turn off the wifi module.
*
* The delay function takes in milliseconds as an argument as opposed to the sleep function which takes in
* seconds. The two values are equal when converted and equate to five minutes. The reason why I trigger the
* delay is because the sleep function is asynchronous; if we didn't have the delay then the next tick
* of the loop would would turn the wifi back on.
*
* Because of the sleep function, updating the firmware on the Photon requires you to put the device into safe mode
* OR you to wait for the fifteen second break in between the five minute intervals.
*/
void loop() {
    Particle.connect();
    Particle.publish("get_travel_time", PRIVATE);
    delay(15000);
    System.sleep(300);
    delay(500000);
}
