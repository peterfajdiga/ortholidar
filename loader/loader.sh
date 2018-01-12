#!/bin/bash
if [[ $# -ne 3 ]]
then
    echo "Usage: loader.sh X Y BLOCK"
    echo "X and Y are top-left coordinates in km"
    echo "BLOCK is the ID of the block that contains the LIDAR data for the chosen coordinates"
    echo "Example: loader.sh 459 100 35"
else
    curl -o input.laz "http://gis.arso.gov.si/lidar/gkot/laz/b_$(($3))/D48GK/GK_$(($1))_$(($2)).laz"
    curl -o input.png "http://gis.arso.gov.si/arcgis/rest/services/DOF_2016/MapServer/export?bbox=$(($1))000.0,$(($2))000.0,$(($1+1))000.0,$(($2+1))000.0&f=image"
    ./ortholidar input.laz input.png output.laz
fi
