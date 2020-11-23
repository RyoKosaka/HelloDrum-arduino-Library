# Sensing Method
## Single Piezo Sensing - singlePiezo(), singlePiezoMUX(), HH(), HHMUX()
<img src="https://open-e-drums.com/images/sensing/single.png" width="800px">  
If the value of the piezo exceeds the threshold, the scan will start. The scan will be done for the scantime you set. The highest piezo value during the scan is the peak. The peak value is converted to a velocity of 0-127. The peak is converted to velocity with the sensitivity value as the maximum value and the threshold value as the minimum value. In other words, if you set the threshold value to the same value as the sensitivity, the velocity will be constant.
Depending on the rigidity of the pad, the piezo vibrates for a while. So, you can set a mask time to prevent retriggering. If you are in mask time, the scan will not start even if the piezo value exceeds the threshold.

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Time (1 - 100) milliseconds
    - Mask Time (1 - 100) milliseconds
    - Velocity Curve Type (0 - 4)

## Dual Piezo Sensing - dualPiezo(), dualPiezoMUX()
<img src="https://open-e-drums.com/images/sensing/dual.png" width="800px">  
Since it's piezo sensing, it's basically the same as single piezo sensing. By comparing the signals from the piezo for the head to the piezo for the rim, we can determine which part of the pad was hit. On most pads, when the head is hit, the piezo for the rim also vibrates. Similarly, the piezo for the head vibrates when the rim is hit. The difference(d) between the peak of the piezo for the rim and the peak of the piezo for the head is used to judge. This can be set in rimSensitivity.  

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Time (1 - 100) milliseconds
    - Mask Time (1 - 100) milliseconds
    - Rim Sensitivity (1 - 100)
    - Rim Threshold (1 - 100)
    - Velocity Curve Type (0 - 4)

## 2-Zone Cymbal Sensing - cymbal2zone(), cymbal2zoneMUX(), HH2zone(), HH2zoneMUX()
<img src="https://open-e-drums.com/images/sensing/cy2.png" width="800px">  
A film switch is used for the edge of the cymbal. This is a quicker response than the piezo. There is no need to scan. Because there is only ON and OFF. To determine the hit position, the value of the film switch at the beginning of the scan and the value of the film switch at the end of the scan are used. If it's a hit to the edge, the film switch will be low by the time the scan is over. On the other hand, if the film switch value is high at the beginning and end of the scan, it is determined to be a choke.  

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Time (1 - 100) milliseconds
    - Mask Time (1 - 100) milliseconds
    - Edge Threshold (1 - 100)
    - Velocity Curve Type (0 - 4)

## 3-Zone Cymbal Sensing - cymbal3zone(), cymbal3zoneMUX()
<img src="https://open-e-drums.com/images/sensing/cy3.png" width="800px">  
It's almost the same as 2-Zone. So far, it only supports YAMAHA's PCY135/155.  
In the PCY135/155, the film switch value is divided by the voltage divider circuit. So, you need to set the Edge Threshold and Cup Threshold. Since the Cup switch has a higher value, set the Cup Threshold higher than the Edge.  
When I checked with the PCY-155  
    Cup Threshold : 4  
    Edge Threshold : 2       
it worked well.

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Time (1 - 100) milliseconds
    - Mask Time (1 - 100) milliseconds
    - Edge Threshold (1 - 100)
    - Cup Threshold (1 - 100)
    - Velocity Curve Type (0 - 4)

## Hihat Controller Sensing - hihatControl(), hihatControlMUX()
<img src="https://open-e-drums.com/images/sensing/hhc.png" width="800px">  
Whether it's the FSR, the infrared sensor (TCRT5000), or Roland's VH-10, they are the same.  
Record the time you started closing the hi-hat and the time you finished closing it. Then, the time it took to close the hi-hat is converted to the velocity. Only ScanStart and ScanEnd and PedalSensitivity are relevant to the velocity. The MIDI CC value can be set in the Sensitivity and Threshold. Velocity Curve here is only applied to MIDI CC values.  

    - Sensitivity (1 - 100)
    - Threshold (1 - 100)
    - Scan Start (1 - 100)
    - Scan End (1 - 100)
    - Pedal Sensitivity (1 - 100)
    - Velocity Curve Type (0 - 4)

## Velocity Curve

 <img src="https://open-e-drums.com/images/sensing/curve.png" width="800px">  