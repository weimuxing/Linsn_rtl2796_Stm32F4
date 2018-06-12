set source="..\Obj\207_Template.axf"
set dest="..\Obj\207_Template.bin"
set exe_path1="C:\Keil\ARM\BIN40\fromelf.exe"
set exe_path2="C:\Keil\ARM\BIN31\fromelf.exe"

if exist %source% (
	if exist %exe_path1% (
		%exe_path1% %source% --bin --output %dest%
	) else (
		if exist %exe_path2% (
			%exe_path2% %source% --bin --output %dest%
		) else (
			echo ERROR: No file %exe_path1% and %exe_path2%
		)
	)	
)else ( 
	echo ERROR: No file %source%
)