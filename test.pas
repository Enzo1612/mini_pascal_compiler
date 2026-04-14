PROGRAM myApp; {this is a {nested CHECK DOUBLE APOSTROPHE ''} comment}
CONST
	x = 'a',
	b = 2;
VAR
	z, t, a;
BEGIN
	WRITE('Type a number: ');
	READ(t);
	z := t * b * 2;
	WRITE('Of block');
	BEGIN
		WRITE('In block');
		WRITE('Your number multiplied by 4 is: ', z); {this is a {nested} comment in the block}
		z := b * z
	END;
	WRITE('Of block');
	WRITE('Your number multiplied by 4 then by itself is: ', z)
END.