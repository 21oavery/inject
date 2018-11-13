; fs:18h -> teb
; teb+30h -> peb
; peb+0Ch -> peb_ldr_data
; peb_ltr_data+0Ch -> mInLoadOrder[0]
; mInLoadOrder[n] -> mInLoadOrder[n+1]
get_peb:
mov eax, 18h
push fs:[eax]
pop eax
push [eax+30h]
pop eax
ret
