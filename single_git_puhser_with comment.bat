@echo off
git status
set /p msg=Added commitment remark:
git add .
git commit -m "%msg%"
git pull
git push -u originGitee master
git push -u origin master
echo Succeed: %msg%
echo --------End!--------
pause