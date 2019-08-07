PROGRAM Blink2
  VAR
    lamp_off : BOOL;
  END_VAR
  VAR
    kill_switch AT %IX0.0 : BOOL;
    lamp1 AT %QX0.0 : BOOL;
    lamp2 AT %QX0.1 : BOOL;
  END_VAR
  VAR
    TON0 : TON;
    TON1 : TON;
  END_VAR

  lamp1 := NOT(kill_switch) AND NOT(lamp_off);
  lamp2 := NOT(kill_switch) AND lamp_off;
  TON0(IN := lamp1, PT := T#1000ms);
  IF TON0.Q THEN
    lamp_off := TRUE; (*set*)
  END_IF;
  TON1(IN := lamp_off, PT := T#1000ms);
  IF TON1.Q THEN
    lamp_off := FALSE; (*reset*)
  END_IF;
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK task0(INTERVAL := T#200ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : Blink2;
  END_RESOURCE
END_CONFIGURATION
