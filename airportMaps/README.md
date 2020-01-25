This folder contains generated airport maps used for airport operations for review and validation.

Airports are divided into three levels internally,

* lvl0 -> No taxi lanes defined, these you will just be told to hold short of the runway (quite a significant number of airports, and the hardest to get ai plane taxi routing right, which often causes the ai planes to fail to initialise)

* lvl1 -> Defined and named taxi lanes, these you will get taxi instructions which must be read back correctly to progress, ai planes should behave well, a few thousand airports. 

* lvl2 -> Hand crafted additional information (such as disabled runways for take off land) and additional testing (currently only EGLL, EGCC and KSEA)
