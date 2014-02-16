var wii = require('./build/Release/wiimote');
var EventEmitter = require('events').EventEmitter;
var util = require('util');

var wiimote = {
	mac : false,
	emitter : false,
	disconnect : function() {
		clearInterval(wiimote.cycle);
		wii.disconnect();
	},
	enableModules : function() {
		wii.enableModules(wiimote.onIr, wiimote.onAcc, wiimote.onBtn, wiimote.onExt);
	},
	onIr : false,
	onAcc : false,
	onBtn : false,
	onExt : false,
	ir : function(on) {
		wiimote.onIr = on;
		wiimote.enableModules();
	},
	acc : function(on) {
		wiimote.onAcc = on;
		wiimote.enableModules();
	},
	btn : function(on) {
		wiimote.onBtn = on;
		wiimote.enableModules();
	},
	ext : function(on) {
		wiimote.onExt = on;
		wiimote.enableModules();
	},
	battery : false,
	getBattery : function() {
		return wiimote.battery;
	},
	acceleration : false,
	getAcc : function() {
		return wiimote.acceleration;
	},
	infrared : false,
	getIr : function() {
		return wiimote.infrared;
	},
	buttons : false,
	getBtn : function() {
		return wiimote.buttons;
	},
	cycle : false
};

var connect = function(mac) {
	var self = this;

	wii.connect(mac);
	wiimote.mac = mac;
	wiimote.enableModules();
	wiimote.cycle = setInterval(function() {
		try
		{
			var data = JSON.parse(wii.getReport());
		
			if(data.battery !== undefined)
			{
				if(wiimote.battery != data.battery)
				{
					self.emit('battery', data.battery);
					wiimote.battery = data.battery;
				}		
			}
			if(data.acceleration !== undefined && wiimote.onAcc)
			{
				self.emit('acceleration', data.acceleration);
				wiimote.acceleration = data.acceleration;
			}
			if(data.ir !== undefined && wiimote.onIr)
			{
				self.emit('ir', data.ir);
				wiimote.ir = data.ir;
			}
			if(data.buttons !== undefined && wiimote.onBtn)
			{
				var i = 0;
				if(data.buttons.A != 0 && wiimote.buttons.A == 0)
				{
					self.emit('A:press');
					i++;
				}
				if(data.buttons.B != 0 && wiimote.buttons.B == 0)
				{
					self.emit('B:press');
					i++;
				}
				if(data.buttons.up != 0 && wiimote.buttons.up == 0)
				{
					self.emit('UP:press');
					i++;
				}
				if(data.buttons.down != 0 && wiimote.buttons.down == 0)
				{
					self.emit('DOWN:press');
					i++;
				}
				if(data.buttons.left != 0 && wiimote.buttons.left == 0)
				{
					self.emit('LEFT:press');
					i++;
				}
				if(data.buttons.right != 0 && wiimote.buttons.right == 0)
				{
					self.emit('RIGHT:press');
					i++;
				}
				if(data.buttons.minus != 0 && wiimote.buttons.minus == 0)
				{
					self.emit('MINUS:press');
					i++;
				}
				if(data.buttons.plus != 0 && wiimote.buttons.plus == 0)
				{
					self.emit('PLUS:press');
					i++;
				}	
				if(data.buttons.home != 0 && wiimote.buttons.home == 0)
				{
					self.emit('HOME:press');
					i++;
				}
				if(data.buttons.one != 0 && wiimote.buttons.one == 0)
				{
					self.emit('1:press');
					i++;
				}
				if(data.buttons.two != 0 && wiimote.buttons.two == 0)
				{
					self.emit('2:press');
					i++;
				}
				if(wiimote.buttons.A != 0 && data.buttons.A == -1)
				{
					self.emit('A:release');
					i++;
				}
				if(wiimote.buttons.B != 0 && data.buttons.B == -1)
				{
					self.emit('B:release');
					i++;
				}
				if(wiimote.buttons.up != 0 && data.buttons.up == -1)
				{
					self.emit('UP:release');
					i++;
				}
				if(wiimote.buttons.down != 0 && data.buttons.down == -1)
				{	
					self.emit('DOWN:release');
					i++;
				}
				if(wiimote.buttons.left != 0 && data.buttons.left == -1)
				{
					self.emit('LEFT:release');
					i++;
				}
				if(wiimote.buttons.right != 0 && data.buttons.right == -1)
				{
					self.emit('RIGHT:release');
					i++;
				}
				if(wiimote.buttons.minus != 0 && data.buttons.minus == -1)
				{
					self.emit('MINUS:release');
					i++;
				}
				if(wiimote.buttons.plus != 0 && data.buttons.plus == -1)
				{
					self.emit('PLUS:release');
					i++;
				}
				if(wiimote.buttons.home != 0 && data.buttons.home == -1)
				{
					self.emit('HOME:release');
					i++;
				}
				if(wiimote.buttons.one != 0 && data.buttons.one == -1)
				{
					self.emit('1:release');
					i++;
				}	
				if(wiimote.buttons.two != 0 && data.buttons.two == -1)
				{
					self.emit('2:release');
					i++;
				}
				if(i > 0)
				{
					self.emit('buttons', data.buttons);
				}
				wiimote.buttons = data.buttons;
			}
		
			wii.clearBtnBuffer();
		}
		catch(e){}
	}, 10);
};


util.inherits(connect, EventEmitter);

connect.prototype.disconnect = wiimote.disconnect;
connect.prototype.ir = wiimote.ir;
connect.prototype.acc = wiimote.acc;
connect.prototype.btn = wiimote.btn;
connect.prototype.getBattery = wiimote.getBattery;
connect.prototype.getAcc = wiimote.getAcc;
connect.prototype.getIr = wiimote.getIr;
connect.prototype.getBtn = wiimote.getBtn;

module.exports = connect;
