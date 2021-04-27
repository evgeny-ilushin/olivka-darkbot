#!/bin/bash
echo "$@" >> /home/olivka/bot/scripts/qify.sh.log

export LC_ALL=ru_RU.cp1251
#export JAVA_HOME=/srv/java/jdk
#export PATH="/srv/java/jdk/bin:$PATH"


#cd /projects/olivka/olivka-tg/core/target
cd ~/src/olivka-tg/core/target
RES=$(java -cp ".:core-1.0-SNAPSHOT-jar-with-dependencies.jar" irc.tula.tg.core.plugin.Qify "$@")

echo "$RES"
echo $RES >> /home/olivka/bot/scripts/qify.sh.log

#java -cp ".:core-1.0-SNAPSHOT-jar-with-dependencies.jar" irc.tula.tg.core.plugin.Qify "$@" 2>&1 | tee -a /home/olivka/bot/scripts/qify.sh.log


