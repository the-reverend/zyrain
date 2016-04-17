z:createTrigger(1, "Type FedComm message")
z:setTriggerReply(1, "hi")
--z:enableTrigger(1)

z:createTrigger(2, "Sub-space comm-link terminated")
z:enableTrigger(2)

z:createTrigger(3, "] (?=Help)? : Q")
z:enableTrigger(3)

repeat
	ans = z:waitForAnyTrigger(500)
	if ans == 1 then z:write("!yay")
		elseif ans == 2 then z:send("/")
		elseif ans == 3 then break
--		elseif ans == 0 then z:write("!!!timeout")
--		elseif ans == -1 then z:write("!!!abort")
		elseif ans == -2 then z:write("!!!badindex")
		elseif ans == -3 then z:write("!!!nop")
		elseif ans == -4 then z:write("!!!fail")
--		else z:write("!!!wtf")
	end
until ans < 0

z:deleteTrigger(1)
z:deleteTrigger(2)
z:deleteTrigger(3)
