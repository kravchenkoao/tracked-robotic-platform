document.addEventListener('DOMContentLoaded', () => {
	
	if (isMobile()) {

		var joy1 = new JoyStick('joy1Div', { // for car's engines control
			externalStrokeColor: '#000',
			internalFillColor: '#000',
		});
		var joy2 = new JoyStick('joy2Div', { // for cam positon x, y control
			externalStrokeColor: '#000',
			internalFillColor: '#000',
		});
	
		setInterval(() => { // continuous connection with server
			
			joy1PosX = joy1.GetX();
			joy1PosY = joy1.GetY();
			joy2PosX = joy2.GetX();
			joy2PosY = joy2.GetY();
			sendPositions(joy1PosX, joy1PosY, joy2PosX, joy2PosY);
		
		}, 300); // middle value between sending too much data & having a long delay

	} else {
		let direction = 0;
		let directionLeft = 0;
		let directionRight = 0;
		let speed = 0;
		let speedForward = 0;
		let speedBackward = 0;
		let camX = 0;
		let camY = 0;

		document.addEventListener('keydown', e => {
			const key = e.key;

			if (key == 'w') {
				speedForward = 100;
			}
			if (key == 's') {
				speedBackward = 100;
			}
			speed = speedForward - speedBackward;
			if (speed < -100) speed = -100;
			else if (speed > 100) speed = 100;

			if (key == 'a') {
				directionLeft = 100;
			}
			if (key == 'd') {
				directionRight = 100;
			}
			direction = directionRight - directionLeft;
			if (direction < -100) direction = -100;
			else if (direction > 100) direction = 100;
		})

		document.addEventListener('keyup', e => {
			const key = e.key;

			if (key == 'w') {
				speedForward = 0;
			}
			if (key == 's') {
				speedBackward = 0;
			}
			speed = speedForward - speedBackward;
			if (speed < -100) speed = -100;
			else if (speed > 100) speed = 100;

			if (key == 'a') {
				directionLeft = 0;
			}
			if (key == 'd') {
				directionRight = 0;
			}
			direction = directionRight - directionLeft;
			if (direction < -100) direction = -100;
			else if (direction > 100) direction = 100;
		})


		setInterval(() => { // continuous connection with server
			
			sendPositions(direction, speed, camX, camY);
		
		}, 300); // middle value between sending too much data & having a long delay
	} 

})

function sendPositions(joy1PosXValue, joy1PosYValue, joy2PosXValue, joy2PosYValue) {
    fetch('/', { // main web page root
		method: "POST",
		headers: {
			"X-CSRFToken": getCookie("csrftoken"), // Django's requirement
		},
		body: JSON.stringify({
			joy1PosX: joy1PosXValue,
			joy1PosY: joy1PosYValue,
			joy2PosX: joy2PosXValue,
			joy2PosY: joy2PosYValue,
		}),
		credentials: 'include',
	});
	console.log('ok');
}

function isMobile() { // for different control styles
	const regex = /Mobi|Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i;
	return regex.test(navigator.userAgent);
}

function getCookie(cname) {
	let name = cname + "=";
	let decodedCookie = decodeURIComponent(document.cookie);
	let ca = decodedCookie.split(';');
	for(let i = 0; i <ca.length; i++) {
		let c = ca[i];
		while (c.charAt(0) == ' ') {
			c = c.substring(1);
		}
		if (c.indexOf(name) == 0) {
		return c.substring(name.length, c.length);
		}
	}
	return "";
}