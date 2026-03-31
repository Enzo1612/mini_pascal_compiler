PROGRAM myApp; {this is a {nested} comment}
CONST
	x = 'a',
	b = 2;
VAR
	z, t;
BEGIN
	WRITE('Type a number: ');
	READ(t);
	z := t * x * 2;
	WRITE('Of block');
	BEGIN
		WRITE('In block');
		WRITE('Your number multiplied by 4 is: ', z); {this is a {nested} comment in the block}
		z := x * z
	END;
	WRITE('Of block');
	WRITE('Your number multiplied by 4 then by itself is: ', z)
END.