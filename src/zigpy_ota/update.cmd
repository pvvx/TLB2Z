set BASE_URL="https://github.com/pvvx/TLB2Z/raw/master/fw"
echo [] > index.json
for %%a in (../bin/*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a %BASE_URL%
)
copy /Y index.json ..\bin\index%1.json
del *.zigbee
del index.json

