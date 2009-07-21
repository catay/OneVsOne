#!/bin/sh

#
# !! ADJUST THE UPPERCASE VARIABLES TO YOUR PREFERENCES !!
#

LOGFILES="/var/tmp/classic.log /var/tmp/hix.log /var/tmp/fancy.log"

CC="some_other_recipient@example.com"
FROM="noreply@example.com"
TO="1vs1@bzleague.com 1vs1@catay.be"

# get yesterdays date
yesterday=$(date --utc -d '1 day ago' +"%Y-%m-%d")

if [[ $(sort ${LOGFILES} | grep -c "${yesterday}") -ne 0 ]]
then
	sort ${LOGFILES} | grep "${yesterday}" |
	mail -a "From: ${FROM}" -s 1vs1MatchSorted -c ${CC} ${TO}
fi

exit 0
