# canigohome

A very small C++ program for the Photon microcontroller that turns the device into a commute time indicator.

```
* The entirety of this program revolves around creating a webhook that performs
* a GET request against the Bing Maps API that returns the duration traveled,
* in seconds, for the route between my work and my home. It then takes said
* duration and uses that to determine whether or not to turn on an LED light as
* an indicator that my commute will take less than sixteen minutes.
```
## Demo

Aw, I can not go home.

![Aw, I can not go home](https://i.imgur.com/LhgBpcOm.jpg)

Heck yes, I *can* go home!

![Heck yes, I *can* go home!](https://i.imgur.com/n8wxruPm.jpg)

# Setup

Dependencies:

* A Photon board.
* An account with [particle.io](https://particle.io)
* A [Bing Maps API key](https://msdn.microsoft.com/en-us/library/ff428642.aspx)
* Raw, dauntless determination.

## Order Your Goods

The beauty of this project was that it was so easy that I went from not knowing what a Photon was to basic project completion in under an hour. All you need to get started is the $19 Photon from [the Particle store](https://store.particle.io/) and some elbow grease. Because the board has an LED light already embedded, there is no need to breadboard anything. However, if you want a stronger light or maybe to put the gadget into a case then, well, you should probably order a soldering iron to go with your Photon.

## Setup your Doohicky

Once you open up your box, follow the [getting started documentation](https://docs.particle.io/guide/getting-started/start/photon/) to get your board communicating with the Particle service. Then go to their [online IDE](https://build.particle.io/build) and see if you can get the light blinking as a testament to your increasingly evident mental fortitude.

Next, you'll need to go procure your very own API key for Bing Maps. Why not the glowing, holy superior Google Maps API? Well, it's because they're stupid. Google is stupid. That's right, I said it. They don't don't allow regular consumer accounts to query for route durations *that include* traffic effects. Why do you have to break my heart like that, Google? Anyways, Both Bing and MapQuest have an API for returning traffic data with their route projections. I'm not a clever man and I was confused by how to get what I wanted from MapQuest's API so I snuggled up to Bing for this project.

## Bing Maps API Request

Once you have your API key, you'll need to figure out the API call to determine the route duration that you want to keep track of. Now, you could go to the API documentation for the routes endpoint buuuuuut I skipped that and went directly to their [example page](https://msdn.microsoft.com/en-us/library/gg636957.aspx).

There, you'll see this beauty: `http://dev.virtualearth.net/REST/V1/Routes/Driving?wp.0=redmond%2Cwa&wp.1=Issaquah%2Cwa&avoid=minimizeTolls&key=BingMapsKey`

It provides directions from Redmond to a place called Issaquah. Fun stuff. Right below that example call, however, is the full JSON body that is returned. It has a whole bunch of unimportant data fields that you have to scroll past but, five years later, you'll get to this returned key value pair: `"travelDurationTraffic":1887`.

![Sweet. Baby. Jesus.](https://media.giphy.com/media/i2j51OF1D2t0c/giphy.gif)

Awh yeah, that's the good stuff. So now you need to figure out how to make that URL work for you; let me break it down.

```
http://dev.virtualearth.net/REST/V1/Routes/Driving?
  \ wp.0=redmond%2Cwa
  \ &wp.1=Issaquah%2Cwa
  \ &key=BingMapsKey
```

That right there is the same URL broken into multiple lines. Let's first review what some of the characters in that URL *do.*

* ? - the question mark says 'here come the options that we want to specify'
* & - the ampersand deliminates (splits up) options
* %2 - the percentage sign followed by a two translates into whitespace (this is relevant when you need to close the gap between the words comprising a location for the URL, ex: San%2Jose)

Alrighty, now lets look at the options we're sending.

* `wp.number=` The WP stands for waypoint, I think. I didn't actually read the documentation and I'm mostly guessing.
* `key=` The API key you need to sign the request.

Alright, so utilizing this treasure trove of precious knowledge, we can construct a new link to fit our needs.

1. Replace the two addresses in the orignal URL that are past the `wp.1=` and `wp.2=` options with your destinations.
2. Insert your key past the `key=` option.
3. Copy and paste the URL into your browser's search bar to check to see if you get a response *as well as* letting your browser replace the white spaces in your URL with the `%20` deliminators.

So, example, say you need to get from The Dog House, FL to Smoothtown, WA. Wait, shoot, those aren't real addresses so I can't be cheeky with my example.

So, example, say you need to get from [No Name, CO](https://en.wikipedia.org/wiki/No_Name,_Colorado) to [Nimrod, MN](https://en.wikipedia.org/wiki/Nimrod,_Minnesota). All you need to do is replace `wp.0=redmond%2Cwa&wp.1=Issaquah%2Cwa` with `wp.0=No Name, CO&wp.1=Nimrod, MN` in the original URL I posted and paste it into your browser.

So that this:

```
http://dev.virtualearth.net/REST/V1/Routes/Driving?wp.0=No Name, CO&wp.1=Nimrod, MN&key=<yeah, like im going to leave my key here>
```

Turns into this:

```http://dev.virtualearth.net/REST/V1/Routes/Driving?wp.0=No%20Name,%20CO&wp.1=Nimrod,%20MN&key=<yeah, like im going to leave my key here>```

You'll end up getting a very large JSON blob back but, whatevers, the capture template functionality Particle provides will filter all that gunk out for you. What's important is you get that `travelDurationTraffic` key back so CTRL-F for it when you test our these routes.

## Webhooks

So the next thing you need to do is create a webhook for your Photon to trigger and subscribe to. This webhook is going to do the heavy lifting of making the `HTTP GET` request against that URL and parsing the JSON so that you only get the travel time under traffic value.

You'll need to go to the [Particle console page](https://console.particle.io/), click on the "integrations" button, create a new webhook, input the URL that you created, paste in this capture template `{{#resourceSets}}{{#resources}}{{travelDurationTraffic}}{{/resources}}{{/resourceSets}}`, and away you go.

Once you have all this setup, flash the source code in this repository onto your device by copy and pasting the contents of `main.cpp` into your online IDE, and then pressing the lightning button. If your Photon is connected to the Internet still, then it should receive that payload of code. Awh, yeah.

Wait! You're not done yet!

The value I'm checking for is less than sixteen minutes because that works for me. Be sure to change that value if it doesn't suit your needs! Maybe you want the threshold for you leaving work to be thirty minutes! Or maybe ten hours! You could be commuting from Zimbabwe to Taiwan via private jet each day for all I know.

## NOTES/TODO/MISC:

* The code could be changed so that, instead of checking the duration, the level of traffic congestion is evaluated instead. This would make the code more portable for other people.
* The Photon is capable of receiving data through an `HTTP POST`. It would be possible to add the ability to the code so that you can overwrite the destinations remotely. Say you work out of two offices, you can use `curl` or Twitter or whatever to change the `wp` values to match wherever you're working from.
* I have a small debug function in there that prints the current time to get home to the logs portion of the Particle console.
* The source code isn't long at all and includes additional documentation. Take a gander at it sometime.
* Overwatch is a really fun game.
