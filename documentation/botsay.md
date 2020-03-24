Dump from "botsay" commands, to be merged with the [AutoATC Phrasebook](https://docs.google.com/document/d/1KEBngY0rwolbH9LJs0wN4NXGxOPSvhJY8ZMKncoliFA/edit?usp=sharing)

Placeholder reference
=====================

In the following chapters "[]" represents a placeholder, to be filled with the agency name or callsign or frequency, etc. used in the current session.

| **Placeholder** | **Meaning** |
--- | ---
| [agency] | The agency that is communicated with, e.g. "Heathrow Tower" or "Frankfurt Approach" |
| [altitude] | The cleared or current altitude, measured in feet e.g. "5000" or flight level e.g. "level 350" |
| [callsign] | Your callsign, e.g. "G-COOL" or "Condor EDC" or "Speedbird 74" ATC may reply with a shortened callsign (See AutoATC ground school) |
| [fix] | Departure or arrival fix that is to be crossed before clearance for the next fix or flight phase. Can be a navigation aid or VFR reporting point |
| [time] | Expresses minutes after the hour, or 4 digit 24 hour time UTC if more than 1 hour in the future |
| [QNH] | QNH, as expressed in hectopascals, e.g. "1006". US and Canada uses inches mercury, e.g. "29.82", readback required |
| [runway] | The runway designator, e.g. "28L", "05" |
| [taxiways] | A list of taxiways, e.g. "Y R M F1" |
| [weatherinfo] | If ATIS present, the current weather information "with november" |
| [squawk] | The code to enter into the transponder, readback required if issued |
| [clearance] | A procedural clearance to a destination when approaching/departing an airport |
| [heading] | A magnetic heading to fly while being vectored by ATC in IFR |

Format is:

*request*

* suggested readback options (dependent upon reply)

**Description**: Heavy aircraft need to request ground service from the gate, this can be done at the same time as starting, alternative request startup then request pushback

*[Agency] [callsign], request push and start [weatherinfo]*

* push back startup approved altimeter [QNH] [callsign]

* push back startup approved Q N H [QNH] [callsign]

* push back startup approved altimeter [QNH] [weatherinfo] [callsign]

* push back startup approved Q N H [QNH] [weatherinfo] [callsign]

*[Agency] [callsign], request start up [weatherinfo]*

* startup approved altimeter [QNH] [callsign]

* startup approved Q N H [QNH] [callsign]

* startup approved altimeter [QNH] [weatherinfo] [callsign]

* startup approved Q N H [QNH] [weatherinfo] [callsign]

* startup approved altimeter [QNH] [weatherinfo] squawk [squawk] [callsign]

* startup approved Q N H [QNH] [weatherinfo] squawk [squawk] [callsign]

* startup approved altimeter [QNH] squawk [squawk] [callsign]

* startup approved Q N H [QNH] squawk [squawk] [callsign]

* startup approved altimeter [QNH] squawk [squawk] [callsign]

* startup approved Q N H [QNH] squawk [squawk] [callsign]

**Description**: Heavy aircraft requesting ground service (needs command adding to trigger better pushback/X-Plane pushback, always approved after startup)

*[Agency] [callsign], request push back*

* push back approved [callsign]

**Description**: Request taxi clearance to the runway, or from the runway to the gate

*[Agency] [callsign], requesting taxi*

*[Agency] [callsign], request taxi*

*[Agency] [callsign], ready to taxi*

* holding position [callsign]

* taxi approved, [taxiways] [callsign]

* taxi approved [taxiways] runway [runway] [callsign]

**Description**: subject to change, reply dependent on previous clearances, to be used to issue IFR [SID] clearance

*[Agency] [callsign], request departure*

*[Agency] [callsign], requesting departure*

* taxi approved [taxiways] runway [runway] [callsign]

* cleared [clearance] line up runway [runway] [callsign]

**Description**: Request runway lineup

*[Agency] [callsign], holding short of runway [runway]*

* holding position [callsign] [callsign]

* cleared [clearance] line up runway [runway] [callsign]

**Description**: request take off (nice explanation of why already in the app ground school)

*[Agency] [callsign],* ready for departure

* holding position [callsign]

* cleared for take off runway [runway], wilco [callsign]

**Description**: inform ATC of the time you will arrive at a waypoint

[Agency] [callsign], estimate [fix] at [time]

* wilco [callsign]

* fly [heading] proceed direct 2 [fix] after [fix] [callsign]

* altimeter [QNH] [callsign]

* Q N H [QNH] [callsign]

**Description**: Inform ATC of your current (precise) location -- use estimate [fix] at [time] if between waypoints.

*[Agency] [callsign], position [fix]*

**Description**: Inform ATC of your next (precise) location, in unplanned flight this will send frequency changes to stay in contact as you go out of range of the current station.

*[Agency] [callsign], next [fix]*

*[Agency] [callsign], next [fix] point at [time]*

*[Agency] [callsign], next [fix] at [time]*

* wilco [callsign]

**Description**: Non standard helper if you are lost, ATC returns distance and magnetic heading (QDM) from your current position to [fix], no read back confirmation expected

*[Agency] [callsign], request vectors to [fix]*

**Description**: When flying a filed IFR flight plan make [fix] the next destination, flightplan updated automatically.

*[Agency] [callsign],* request direct to [fix]

**Description**: Request permission to approach an airport and land

*[Agency] [callsign], estimate [fix] at [time] request join [weatherinfo]*
fix
* holding at [fix] [callsign]

* [altitude] hold at [fix] expecting runway [runway] [callsign]

* cleared for approach runway [runway] fly [heading] [callsign]

* cleared for approach runway [runway] wilco [callsign]

**Description**: Inform ATC you are in the landing pattern

*[Agency] [callsign], established runway in sight*

*[Agency] [callsign], downwind*

*[Agency] [callsign], aerodrome in sight*

* cleared 2 approach runway [runway] report final

* fly [heading] wilco

* wilco

**Description**: Inform ATC you are on a 5/10 mile final approach

*[Agency] [callsign],* outer marker

*[Agency] [callsign],* final

* go around [callsign]

* cleared 2 land, runway [runway] [callsign]
