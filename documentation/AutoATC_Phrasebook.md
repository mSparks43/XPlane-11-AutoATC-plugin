# AutoATC Phrasebook
&nbsp;

This is a quick reference phrasebook for use with AutoATC, a combined AI/ATC plugin and radio communication training tool for X-Plane [(X-Plane.org file library page)](https://forums.x-plane.org/index.php?/files/file/45663-main-installation-files-for-autoatc-for-xplane-11/).

This phrasebook is a mere quick reference aid for pilots. Its content is subject to change as AutoATC is still under heavy development.

&nbsp;

&nbsp;

## 1. Placeholders
In the following chapters, “[]” represents a placeholder, which is to be replaced with the agency name or callsign or frequency, etc. used in the current session.

&nbsp;

>**[agency]**: The agency that is communicated with, e.g. "Heathrow Tower" or "Frankfurt Approach"

>**[altitude]**: The cleared or current altitude, measured in feet e.g. "5000" or flight level e.g. "level 350"

>**[callsign]**: Your callsign, e.g. "G-COOL" or "Condor EDC" or "Speedbird 74" ATC may reply with a shortened callsign (see AutoATC ground school)

>**[clearance]**: A procedural clearance to a destination when approaching/departing an airport

>**[fix]**: Departure or arrival fix that is to be crossed before clearance for the next fix or flight phase. Can be a navigation aid or VFR reporting point

>**[heading]**: A magnetic heading to fly while being vectored by ATC in IFR

>**[QNH]**: QNH, as expressed in hectopascals, e.g. "1006". US and Canada uses inches mercury, e.g. "29.82"

>**[runway]**: The runway designator, e.g. "28L", "05"

>**[squawk]**: Transponder code, e.g. "2168"

>**[taxiways]**: A list of taxiways, e.g. "Y R M F1"

>**[time]**: Expresses minutes after the hour, or 4 digit 24 hour time UTC if more than 1 hour in the future

>**[weatherinfo]**: *If the airport has ATIS*, this is the current weather information, e.g. "with november"

&nbsp;

## 2. Communications
Communications are presented in this form:



**Note:** Further information about a request, such as required conditions are mentioned in a note below the respective heading.

**Request:**
>The request that invokes a reply by ATC. Variations of a request are indicated with an asterisk
	
**Readback:**
>If a request lists one or more readbacks, ATC replies will have to be read back. The correct readback depends on the content of the reply by ATC.

&nbsp;

### 2.1 General
#### 2.1.1 Radio check
**Request:** 
>[Agency] [callsign], request radio check

&nbsp;

#### 2.1.2 Registering a callsign
**Request:** 
>[Agency] [callsign], good day

&nbsp;

### 2.2 Start-up
#### 2.2.1 Requesting start-up with pushback
**Note:** Requesting pushback from a gate can be done in one go with this request or separately (see below)

**Request:** 
>[Agency], [callsign], request push and start [weatherinfo].

**Readback:**
>* Push back and startup approved, altimeter [QNH], [callsign].
* Push back and startup approved, QNH [QNH], [callsign].
* Push back and startup approved, altimeter [QNH], [weatherinfo], [callsign].
* Push back and startup approved, QNH [QNH], [weatherinfo],[callsign].

&nbsp;

#### 2.2.2 Requesting start-up
**Note:** When requiring pushback, request it in a separate call or request startup using the call above

**Request:** 
>[Agency] [callsign], request start up [weatherinfo].

**Readback:**
>* Startup approved, altimeter [QNH], [callsign].
* Startup approved, QNH [QNH], [callsign].
* Startup approved, altimeter [QNH], [weatherinfo], [callsign].
* Startup approved, QNH [QNH], [weatherinfo], [callsign].
* Startup approved, altimeter [QNH], [weatherinfo], squawk [squawk], [callsign].
* Startup approved, QNH [QNH], [weatherinfo], squawk [squawk], [callsign].
* Startup approved, altimeter [QNH], squawk [squawk], [callsign].
* Startup approved, QNH [QNH], squawk [squawk], [callsign].
* Startup approved, altimeter [QNH], squawk [squawk], [callsign].
* Startup approved, QNH [QNH], squawk [squawk], [callsign].

&nbsp;

#### 2.2.3 Requesting pushback
**Note:** Startup should be requested before (see above)

**Request:** 
>[Agency] [callsign], request push back.

**Readback:**
>Push back approved, [callsign].

&nbsp;

### 2.3 Taxi
#### 2.3.1 Requesting taxi clearance
**Note:** Applies to taxiing from parking/gate to the runway or from the runway to the gate/parking

**Request:** 
>* [Agency] [callsign], requesting taxi.
* [Agency] [callsign], request taxi.
* [Agency] [callsign], ready to taxi.

**Readback:**
>* Holding position, [callsign].
* Taxi approved, via [taxiways], [callsign].
* Taxi approved, via [taxiways], runway [runway], [callsign].

&nbsp;

### 2.4 Departure/Line-Up
#### 2.4.1 Requesting departure
**Note:** Subject to change; to be used to issue IFR (SID) clearance

**Request:**
>* [Agency] [callsign], request departure.
* [Agency] [callsign], requesting departure.
	
**Readback:**
>Cleared [clearance], line up runway [runway], [callsign].
	
&nbsp;

#### 2.4.2 Requesting runway lineup
**Request:**
>[Agency] [callsign], holding short of runway [runway].
	
**Readback:**
>* Holding position, [callsign], 
* Cleared [clearance], line up runway [runway], [callsign].
	
&nbsp;


### 2.5 Take-off
#### 2.5.1 Requesting takeoff
**Note:** Departure clearance does *not* mean take-off clearance, unless you're a KLM Boeing 747

**Request:**
>[Agency], [callsign], ready for departure.
	
**Readback:**
>* Holding position, [callsign].
* Cleared for takeoff, runway [runway], wilco [callsign].
	
&nbsp;


### 2.6 Navigation
#### 2.6.1 Reporting waypoint arrival time
**Request:**
>[Agency], [callsign], estimate [fix] at [time].
	
**Readback:**
>* Wilco, [callsign].
* Fly [heading], proceed direct to [fix] after [fix], [callsign].
* Altimeter [QNH], [callsign].
* Q N H [QNH], [callsign].

&nbsp;

#### 2.6.2 Reporting the current position
**Request:**
>[Agency], [callsign], position [fix].

&nbsp;

#### 2.6.3 Reporting the next position
**Note:** This informs ATC of the next, precise location. When making an unplanned flight, this will invoke frequency changes to stay in contact once you go out of the agency's radio range.

**Request:**
>* [Agency], [callsign], next [fix].
* [Agency], [callsign], next [fix], point at [time].
* [Agency], [callsign], next [fix] at [time].
	
**Readback:**
>Wilco, [callsign].
	
&nbsp;

#### 2.6.4 Requesting vectors to a fix
**Note:** Useful when getting lost. ATC replies with distance and heading from your current position to the desired fix.

**Request:**
>[Agency], [callsign], request vectors to [fix].
	
&nbsp;

#### 2.6.5 Requesting a direct-to
**Note:** Only when flying a filed IFR flight plan. Makes the requested fix the next destination and updates the flightplan automatically.

**Request:**
>[Agency], [callsign], request direct to [fix].
	
&nbsp;

### 2.7 Approach/Landing
#### 2.7.1 Requesting approach and landing
**Request:**
>[Agency], [callsign], estimate [fix] at [time], request join,  [weatherinfo].
	
**Readback:**
>* Holding at [fix], [callsign].
* Altitude [altitude], hold at [fix], expecting runway [runway], [callsign].
* Cleared for approach, runway [runway], fly [heading], [callsign].
* Cleared for approach, runway [runway], wilco [callsign].

&nbsp;

#### 2.7.2 Reporting established (approach/pattern)
**Request:**
>* [Agency], [callsign], established, runway in sight.
* [Agency], [callsign], downwind.
* [Agency], [callsign], aerodrome in sight.
	
**Readback:**
>* Cleared to approach, runway [runway], report final, [callsign].
* Fly heading [heading], wilco, [callsign].
* Wilco, [callsign].

&nbsp;
	
#### 2.7.3 Reporting final
**Request:**
>* [Agency], [callsign], outer marker.
* [Agency], [callsign], final.
	
**Readback:**
>* Go around, [callsign].
* Cleared to land, runway [runway], [callsign].