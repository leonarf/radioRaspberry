function(replaceTag tag value filePath)
	FILE(READ ${filePath} FILE_CONTENT)
	MESSAGE("replace ${tag} by ${value} in file ${filePath}")
	STRING(REPLACE "${tag}" "${value}" 
		MODIFIED_FILE_CONTENT "${FILE_CONTENT}")
	FILE(WRITE ${filePath} "${MODIFIED_FILE_CONTENT}")
endfunction(replaceTag)

