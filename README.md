<b>Embedded Systems - Skill test Alarm Clock</b>

Group Members:<br>
		Devon Briere, 
		Hansen William, 
		Nathan Dhami, 
		NoorUllah Randhawa, 
		
<br>Pre-requisites:</br>
<ul>
<li>Latest node version</li>
<li>We use mongodb to persist data, download from: https://docs.mongodb.com/manual/tutorial/install-mongodb-on-ubuntu/</li>
</ul>

	
<br>Instructions to compile and run:<br>
<ul>
<li>From the host, go into the project folder (`cd ~/cmpt433/work/CMPT433-Project`) and run `make`</li>
<li>From the host, go into the public directory for the node application (`cd ~/cmpt433/public/myApps/wakeup-server-copy`), and run `npm install`</li>
<li>Need to start a mongodb instance on the host. In a terminal on the host, run `mongod` to start the mongo instance</li>
<li>On the target, open two terminals</li>
    <ul>
    <li>In one terminal, go to the mounted folder for apps (`cd ~/cmpt433/public/myApps`), and run `./alarmer`</li>
    <li>In the second terminal, go to the mounted folder and into the node server folder (`cd /mnt/remote/myApps/wakeup-server-copy`), 
    and run `node bin/www` to start the application</li>
    <li>Browse the application on http://localhost:8088</li>
    </ul>
</ul>