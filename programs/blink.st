PROGRAM Blink
  VAR
    lamp_off : BOOL;
  END_VAR
  VAR
    lamp AT %QX0.0 : BOOL;
  END_VAR
  VAR
    TON0 : TON;
    TON1 : TON;
  END_VAR

  lamp := NOT(lamp_off);
  TON0(IN := lamp, PT := T#1s);
  IF TON0.Q THEN
    lamp_off := TRUE; (*set*)
  END_IF;
  TON1(IN := lamp_off, PT := T#1s);
  IF TON1.Q THEN
    lamp_off := FALSE; (*reset*)
  END_IF;
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK task0(INTERVAL := T#200ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : Blink;
  END_RESOURCE
END_CONFIGURATION
