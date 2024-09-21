#include "controller.h"


Controller::Controller(float kp,float ki, float kd)
{   KP = kp;
    KD = kd;
    KI = ki;
    integral = 0;
    time = 0 ; 
    last_time = 0 ;
    delta_time= 0 ;
    error = 0;
    last_error = 0;
}

float Controller::output(float setpoint, float current_value){

    time = millis();
    setpoint_ = setpoint; //*3.1415/180;
    current_value_ = current_value;

    error =  setpoint_ - current_value_ ;
    Serial.print("ERRO = ");
    Serial.print(error);
    // Serial.print(" | Angulo: ");

    if (error < 0) error *= -1; // Faz modulo do error, para error ser sempre possitivo

    if(error >= 0 && error <= 45*3.1415/180){
        KP = 3; //constante correção de erros PID
        KI = 0.01;
        KD = 0.0;
        // Serial.println("45");
    }
    if(error > 45*3.1415/180 && error <= 90*3.1415/180){
        KP = 1.6; //constante correção de erros PID
        KI = 0.01;
        KD = 0.0;
        // Serial.println("90");
    }
    if(error > 90*3.1415/180 && error <= 135*3.1415/180){
        KP = 1.18; //constante correção de erros PID
        KI = 0.01;
        KD = 0.0;
        // Serial.println("135");
    }
    if(error > 135*3.1415/180 && error <= 180*3.1415/180){
        KP = 1.088; //constante correção de erros PID
        KI = 0.01;
        KD = 0.0;
        // Serial.println("180");
    }
    else {

        // Serial.println("deu ruim");
    }

    delta_time = (double)(time - last_time)/10000;  //
    // if(setpoint_ == 0){
    //     output_value =  current_value_ + proportional() + derivative() ;
    // }else{
    // }

    output_value =  proportional() + integrative() + derivative();
    // output_value = saturation(output_value,1000);

    last_time = time;
    last_error = error; // Guarda o erro antigo
  
    return output_value;
}

float Controller::proportional(){

    return error*KP; 

}

float Controller::integrative(){
    integral += error*delta_time;

    // integral = saturation(integral,1000);
    return integral*KI;

}

float Controller::derivative(){
   
    if (last_error - error!= 0){
        delta_error = (last_error - error)/delta_time;
    }
    else {
        delta_error = 0; 
    }
    return delta_error*KD;

}

void Controller::debug(){
    
    // Serial.print(" |delta_time: ");
    // Serial.print(delta_time);

    Serial.print("|input_values: ");
    Serial.print(setpoint_);
    Serial.print("|current_values: ");
    Serial.print(current_value_);

    Serial.print("||error: ");
    Serial.print(error);
    Serial.print("|P: ");
    Serial.print(proportional());
    Serial.print("|I: ");
    Serial.print(integrative());
    Serial.print("|D: ");
    Serial.print(derivative());

    Serial.print("|output_value: ");
    Serial.println(output_value);
    
    // Serial.println("");

}




