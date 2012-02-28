define sb
break break_one
break break_two
end

document sb
set break on break_one() and break_two()
end

define bye
kill
quit
end

document bye
Kill running program and quit.
end

define bt2
thread 2
bt
end

document bt2
Switch to thread 2 and print backtrace
end
