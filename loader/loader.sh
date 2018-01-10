curl -o input.laz "http://gis.arso.gov.si/lidar/gkot/laz/b_37/D48GK/GK_$(($1))_$(($2)).laz"
curl -o input.png "http://gis.arso.gov.si/arcgis/rest/services/DOF_2016/MapServer/export?bbox=$(($1))000.0,$(($2))000.0,$(($1+1))000.0,$(($2+1))000.0&f=image"
./Ortholidar input.laz input.png output.laz
