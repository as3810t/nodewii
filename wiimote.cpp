#include <iostream>
#include <vector>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <cmath>
#include <node.h>
#include <v8.h>
#include <float.h>
extern "C" {
	#include <cwiid.h>
	#include <bluetooth/bluetooth.h>
}

#define PI	3.14159265358979323
#define MAC 00:22:AA:8F:20:50

using namespace v8;
using namespace std;

template <typename T>
  string numToStr ( T Number )
  {
     ostringstream ss;
     ss << Number;
     return ss.str();
  }

cwiid_mesg_callback_t cwiid_callback;

cwiid_wiimote_t* wiimote;
struct cwiid_state state;
struct acc_cal wm_cal;
bdaddr_t mac;

string resultAcc = "";
string resultIr = "";
string resultBtn = "";
string resultStatus = "";
vector<int> buttons(11);

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count, union cwiid_mesg *mesg_array, struct timespec *timestamp)
{
	double ax, ay, az, roll, pitch;
	
	static enum cwiid_ext_type ext_type = CWIID_EXT_NONE;

	for(int i = 0; i < mesg_count; i++)
	{
		switch(mesg_array[i].type)
		{
			case CWIID_MESG_STATUS:
				resultStatus = "\"battery\": " + numToStr(100.0 * mesg_array[i].status_mesg.battery / CWIID_BATTERY_MAX);
				break;
			case CWIID_MESG_BTN:
				resultBtn = "\"buttons\": {";
				resultBtn += "\"A\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_A) ? (buttons[0] = 1) : (buttons[0] != 0 ? (buttons[0] = -1) : (buttons[0] = 0)));
				resultBtn += ", ";
				resultBtn += "\"B\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_B) ? (buttons[1] = 1) : (buttons[1] != 0 ? (buttons[1] = -1) : (buttons[1] = 0)));
				resultBtn += ", ";
				resultBtn += "\"up\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_UP) ? (buttons[2] = 1) : (buttons[2] != 0 ? (buttons[2] = -1) : (buttons[2] = 0)));
				resultBtn += ", ";
				resultBtn += "\"down\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_DOWN) ? (buttons[3] = 1) : (buttons[3] != 0 ? (buttons[3] = -1) : (buttons[3] = 0)));
				resultBtn += ", ";
				resultBtn += "\"left\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_LEFT) ? (buttons[4] = 1) : (buttons[4] != 0 ? (buttons[4] = -1) : (buttons[4] = 0)));
				resultBtn += ", ";
				resultBtn += "\"right\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_RIGHT) ? (buttons[5] = 1) : (buttons[5] != 0 ? (buttons[5] = -1) : (buttons[5] = 0)));
				resultBtn += ", ";
				resultBtn += "\"minus\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_MINUS) ? (buttons[6] = 1) : (buttons[6] != 0 ? (buttons[6] = -1) : (buttons[6] = 0)));
				resultBtn += ", ";
				resultBtn += "\"plus\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_PLUS) ? (buttons[7] = 1) : (buttons[7] != 0 ? (buttons[7] = -1) : (buttons[7] = 0)));
				resultBtn += ", ";
				resultBtn += "\"home\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_HOME) ? (buttons[8] = 1) : (buttons[8] != 0 ? (buttons[8] = -1) : (buttons[8] = 0)));
				resultBtn += ", ";
				resultBtn += "\"one\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_1) ? (buttons[9] = 1) : (buttons[9] != 0 ? (buttons[9] = -1) : (buttons[9] = 0)));
				resultBtn += ", ";
				resultBtn += "\"two\": ";
				resultBtn += numToStr(((&mesg_array[i].btn_mesg)->buttons & CWIID_BTN_2) ? (buttons[10] = 1) : (buttons[10] != 0 ? (buttons[10] = -1) : (buttons[10] = 0)));
				resultBtn += "}";
				break;
			case CWIID_MESG_ACC:			
				ax = ((double) (&mesg_array[i].acc_mesg)->acc[CWIID_X] - wm_cal.zero[CWIID_X]) / (wm_cal.one[CWIID_X] - wm_cal.zero[CWIID_X]);
				ay = ((double) (&mesg_array[i].acc_mesg)->acc[CWIID_Y] - wm_cal.zero[CWIID_Y]) / (wm_cal.one[CWIID_Y] - wm_cal.zero[CWIID_Y]);
				az = ((double) (&mesg_array[i].acc_mesg)->acc[CWIID_Z] - wm_cal.zero[CWIID_Z]) / (wm_cal.one[CWIID_Z] - wm_cal.zero[CWIID_Z]);
				roll = -1 * (atan(ax / az) + (az <= 0 ? (PI * ((ax > 0) ? 1 : -1)) : 0));
				pitch = atan(ay / az * cos(roll));

				resultAcc = "\"acceleration\": {";
				resultAcc += "\"x\": " + numToStr(ax) + ", ";
				resultAcc += "\"y\": " + numToStr(ay) + ", ";
				resultAcc += "\"z\": " + numToStr(az) + ", ";
				resultAcc += "\"acc\": " + numToStr(sqrt(pow(ax, 2) + pow(ay, 2) + pow(az, 2))) + ", ";
				resultAcc += "\"roll\": " + numToStr(roll) + ", ";
				resultAcc += "\"pitch\": " + numToStr(pitch) + "}";
				break;
			case CWIID_MESG_IR:
				if(cwiid_get_state(wiimote, &state))
				{
					return;
				}

				resultIr = "\"ir\": [";
				for(int j = 0; j < CWIID_IR_SRC_COUNT; j++)
				{
					if(j!=0)
					{
						resultIr += ", ";
					}
					resultIr += "[" + numToStr(state.ir_src[j].pos[CWIID_X]) + ", " + numToStr(state.ir_src[j].pos[CWIID_Y]) + "]";
				}
				resultIr += "]";
				break;
			case CWIID_MESG_ERROR:
				return;
				break;
			default:
				break;
		}
	}
	return;
}


Handle<Value> connect(const Arguments& args)
{
    HandleScope scope;
  	
	String::Utf8Value param1(args[0]->ToString());
    string macAdd = string(*param1);
  
	str2ba(macAdd.c_str(), &mac);
    if(!(wiimote = cwiid_open(&mac, CWIID_FLAG_MESG_IFC)))
    {
		return ThrowException(Exception::Error(String::New("Unable to connect to the device")));
	}
	else if(cwiid_set_mesg_callback(wiimote, &cwiid_callback))
	{
		return ThrowException(Exception::Error(String::New("Unable to configure message callback")));
	}
	
	if(cwiid_get_acc_cal(wiimote, CWIID_EXT_NONE, &wm_cal))
	{
		return ThrowException(Exception::Error(String::New("Unable to calibrate the acceleration sensor")));
	}
	cwiid_set_rpt_mode(wiimote, CWIID_RPT_STATUS);
	cwiid_request_status(wiimote);
    
	return scope.Close(Integer::New(1));
}

Handle<Value> disconnect(const Arguments& args)
{
	HandleScope scope;
		
	if(cwiid_close(wiimote))
	{
		return ThrowException(Exception::Error(String::New("Unable to close the connection")));
	}
		
	return scope.Close(Integer::New(0));
}

Handle<Value> rumble(const Arguments& args)
{
    HandleScope scope;
  
    int32_t in = args[0]->ToInteger()->Value();
    unsigned char on = in ? 1 : 0;
    
	if(cwiid_set_rumble(wiimote, on))
	{
		return ThrowException(Exception::Error(String::New("Unable to set rumble")));
	}
    return scope.Close(Integer::New(0));
}

Handle<Value> leds(const Arguments& args)
{
    HandleScope scope;
  
    int index = args[0]->ToInteger()->Value();
    bool on = args[1]->ToBoolean()->Value();
    
    int indexes[] = { CWIID_LED1_ON, CWIID_LED2_ON, CWIID_LED3_ON, CWIID_LED4_ON };
    cwiid_get_state(wiimote, &state);
   
    int led = state.led;
    led = on ? led | indexes[index-1] : led ^ indexes[index-1];
    
	if(cwiid_set_led(wiimote, led))
	{
		return ThrowException(Exception::Error(String::New("Unable to set leds")));
	}
   return scope.Close(Integer::New(0));
}

Handle<Value> enableModules(const Arguments& args)
{
    HandleScope scope;
  
    bool onIr = args[0]->ToBoolean()->Value();
    bool onAcc = args[1]->ToBoolean()->Value();
    bool onButtons = args[2]->ToBoolean()->Value();
    bool onExt = args[3]->ToBoolean()->Value();
    
    uint8_t mods = 0;
    
    if(onIr){mods |= CWIID_RPT_IR;}
    if(onAcc){mods |= CWIID_RPT_ACC;}
    if(onButtons){mods |= CWIID_RPT_BTN;}
    //if(onExt){mods |= CWIID_RPT_EXT;}

	if(cwiid_set_rpt_mode(wiimote, mods | CWIID_RPT_STATUS))
	{
		return ThrowException(Exception::Error(String::New("Unable to set report mode")));
	}

	return scope.Close(Integer::New(0));
}

Handle<Value> getReport(const Arguments& args)
{
    HandleScope scope;
    
    string result = "{";
    
    if(resultAcc != "")
    {
    	result += resultAcc + ", ";
    }
    if(resultIr != "")
    {
    	result += resultIr + ", ";
    }
    if(resultBtn != "")
    {
    	result += resultBtn + ", ";
    }
    if(resultStatus != "")
    {
    	result += resultStatus + ", ";
    }
    
    result = result.substr(0, result.size() - 2) + "}";
	return scope.Close(String::New(result.c_str()));
}

Handle<Value> clearBtnBuffer(const Arguments& args)
{
    HandleScope scope;
    
    string id = "\"buttons\": {\"A\": ";
    id += numToStr(buttons[0] == -1 ? (buttons[0] = 0) : buttons[0]);
    id += ", ";
    id += "\"B\": ";
    id += numToStr(buttons[1] == -1 ? (buttons[1] = 0) : buttons[1]);
    id += ", ";
    id += "\"up\": ";
    id += numToStr(buttons[2] == -1 ? (buttons[2] = 0) : buttons[2]);
    id += ", ";
    id += "\"down\": ";
    id += numToStr(buttons[3] == -1 ? (buttons[3] = 0) : buttons[3]);
    id += ", ";
    id += "\"left\": ";
    id += numToStr(buttons[4] == -1 ? (buttons[4] = 0) : buttons[4]);
    id += ", ";
    id += "\"right\": ";
    id += numToStr(buttons[5] == -1 ? (buttons[5] = 0) : buttons[5]);
    id += ", ";
    id += "\"minus\": ";
    id += numToStr(buttons[6] == -1 ? (buttons[6] = 0) : buttons[6]);
    id += ", ";
    id += "\"plus\": ";
    id += numToStr(buttons[7] == -1 ? (buttons[7] = 0) : buttons[7]);
    id += ", ";
    id += "\"home\": ";
    id += numToStr(buttons[8] == -1 ? (buttons[8] = 0) : buttons[8]);
    id += ", ";
    id += "\"one\": ";
    id += numToStr(buttons[9] == -1 ? (buttons[9] = 0) : buttons[9]);
    id += ", ";
    id += "\"two\": ";
    id += numToStr(buttons[10] == -1 ? (buttons[10] = 0) : buttons[10]);
    id += "}";
    resultBtn = id;
    
	return scope.Close(Integer::New(0));
}


void init(v8::Handle<v8::Object> target)
{
	target->Set(String::NewSymbol("connect"), FunctionTemplate::New(connect)->GetFunction());
	target->Set(String::NewSymbol("disconnect"), FunctionTemplate::New(disconnect)->GetFunction());
	target->Set(String::NewSymbol("rumble"), FunctionTemplate::New(rumble)->GetFunction());
	target->Set(String::NewSymbol("led"), FunctionTemplate::New(leds)->GetFunction());
	target->Set(String::NewSymbol("getReport"), FunctionTemplate::New(getReport)->GetFunction());
	target->Set(String::NewSymbol("enableModules"), FunctionTemplate::New(enableModules)->GetFunction());
	target->Set(String::NewSymbol("clearBtnBuffer"), FunctionTemplate::New(clearBtnBuffer)->GetFunction());
}


NODE_MODULE(wiimote, init);
