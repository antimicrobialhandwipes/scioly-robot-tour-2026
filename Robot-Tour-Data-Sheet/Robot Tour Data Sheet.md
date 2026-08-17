**\*\*Robot Tour Movement Data Sheet:\*\***

Disclaimers: 

\* These are all manual calibrations based off the robot’s physical conditions (length and width)  
\* forward(), backward(), forwardUntil(), until() are all measurements based off the dowel  
\* Movement functions have integer arguments, so decimals get truncated

| Situation: | Before: | After: | Code: |
| :---- | :---- | :---- | ----- |
| **\*\*→ first move:\*\*** Positioned in the middle of the grid | ![image1](Robot-Tour-Data-Sheet/images/image1.png) | ![image2](Robot-Tour-Data-Sheet/images/image2.png) | forward(25); |
| **\*\*→ first move:\*\*** Positioned in the middle of the grid | ![image3](Robot-Tour-Data-Sheet/images/image3.png) | ![image4](Robot-Tour-Data-Sheet/images/image4.png) | forwardUntil(17); |
| Positioned in the middle of the grid | ![image5](Robot-Tour-Data-Sheet/images/image5.png) | ![image6](Robot-Tour-Data-Sheet/images/image6.png) | until(17); |
| Positioned to the middle of the box before turning to move a bottle | ![image7](Robot-Tour-Data-Sheet/images/image7.png) | ![image8](Robot-Tour-Data-Sheet/images/image8.png) | forward(25); right(); / left(); |
| Positioned to the middle of the box before turning to move a bottle | ![image9](Robot-Tour-Data-Sheet/images/image9.png) | ![image10](Robot-Tour-Data-Sheet/images/image10.png) | forwardUntil(17); right(); / left(); |
| Positioned to the middle of the box before turning to move a bottle | ![image11](Robot-Tour-Data-Sheet/images/image11.png) | ![image12](Robot-Tour-Data-Sheet/images/image12.png) | until(17); right(); / left(); |
| Use l180 to move a bottle from an inaccessible area and positioning it back to move a bottle to the center of the grid | ![image13](Robot-Tour-Data-Sheet/images/image12.png) | ![image14](Robot-Tour-Data-Sheet/images/image13.png) | fw(50); l180(); / r180(); fw(50); |