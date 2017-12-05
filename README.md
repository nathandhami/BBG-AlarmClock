<b>Embedded Systems - Skill test Alarm Clock</b>

Group Members:<br>
		Devon Briere, 
		Hansen William, 
		Nathan Dhami, 
		NoorUllah Randhawa, 

	
<br>Instructions to compile and run:<br>
<ul>
<li>From the host, go into the project folder (`cd ~/cmpt433/work/CMPT433-Project`) and run `make`</li>
<li>From the host, go into the public directory for the node application (`cd ~/cmpt433/public/myApps/wakeup-server-copy`), and run `npm install`</li>
<li>On the target, open two terminals</li>
    <ul>
    <li>In one terminal, go to the mounted folder for apps (`cd ~/cmpt433/public/myApps`), and run `./alarmer`</li>
    <li>In the second terminal, go to the mounted folder and into the node server folder (`cd /mnt/remote/myApps/wakeup-server-copy`), 
    and run `node bin/www` to start the application</li>
    <li>Browse the application on http://localhost:8088</li>
    </ul>
</ul>