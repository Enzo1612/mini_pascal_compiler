PROGRAM controlFlowTest;
{ Validates WHILE loops, IF/THEN/ELSE branching, and arithmetic conditions }
CONST
    step = 1,
    two = 2;
VAR
    counter, temp;
BEGIN
    WRITE('Enter a countdown start value: ');
    READ(counter);

    WHILE counter DO
    BEGIN
        temp := counter;

        IF temp - ((temp / two) * two) THEN
        BEGIN
            WRITE('The number is ODD: ', counter)
        END
        ELSE
        BEGIN
            WRITE('The number is EVEN: ', counter)
        END;

        counter := counter - step
    END;

    WRITE('Execution complete. Loop terminated successfully.')
END.