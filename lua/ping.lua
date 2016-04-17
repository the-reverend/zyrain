timeout = 5000

z:createTrigger( 1, "S:" )
z:enableTrigger( 1 )
z:send( "'\r" )
z:waitForTrigger( 1, timeout )
z:deleteTrigger( 1 )

z:createTrigger( 2, "S: ping" )
z:enableTrigger( 2 )

count = 4
c = 0
min = timeout
max = 0

t0 = z.tick

z:send( "ping" )
t1 = t0

repeat
	ans = z:waitForTrigger( 2, timeout )
	c = c + 1
	t2 = z.tick
	delta = t2 - t1
	min = math.min( min, delta )
	max = math.max( max, delta )
	t1 = t2
	z:send( string.format( " : %d ms\rping", delta ) )
until ans < 1 or c >= count

z:disableTrigger( 2 )
z:send( string.format( " : count=%d : min=%d : avg=%d : max=%d\r\r",
	c, min, ( t2 - t0 ) / c, max ) )

z:deleteTrigger( 2 )
