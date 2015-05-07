//0  - 15 very weak
//16 - 24 weak
//25 - 34 mediocre
//35 - 44 strong
//   < 44 stronger
function pwdStrength(passwd)
{
		var intScore   = 0
				
		// PASSWORD LENGTH
		if (passwd.length<5)                         // length 4 or less
		{
			intScore = (intScore+3)
		}
		else if (passwd.length>4 && passwd.length<8) // length between 5 and 7
		{
			intScore = (intScore+6)
		}
		else if (passwd.length>7 && passwd.length<16)// length between 8 and 15
		{
			intScore = (intScore+12)
		}
		else if (passwd.length>15)                    // length 16 or more
		{
			intScore = (intScore+18)
		}
		// LETTERS (Not exactly implemented as dictacted above because of my limited understanding of Regex)
		if (passwd.match(/[a-z]/))                              // [verified] at least one lower case letter
		{
			intScore = (intScore+1)
		}
		if (passwd.match(/[A-Z]/))                              // [verified] at least one upper case letter
		{
    		intScore = (intScore+5)
		}
		// NUMBERS
		if (passwd.match(/\d+/))                                 // [verified] at least one number
		{
			intScore = (intScore+5)
	    }
		if (passwd.match(/(.*[0-9].*[0-9].*[0-9])/))             // [verified] at least three numbers
		{
			intScore = (intScore+5)
		}
		// SPECIAL CHAR
//		if (passwd.match(/.[!,@,#,$,%,^,&,*,?,_,~]/))            // [verified] at least one special character
//		{
//			intScore = (intScore+5)
//		}
        // [verified] at least two special characters
//		if (passwd.match(/(.*[!,@,#,$,%,^,&,*,?,_,~].*[!,@,#,$,%,^,&,*,?,_,~])/))
//		{
//			intScore = (intScore+5)
//		}
		// COMBOS
		if (passwd.match(/([a-z].*[A-Z])|([A-Z].*[a-z])/))        // [verified] both upper and lower case
		{
			intScore = (intScore+2)
		}
		if (passwd.match(/([a-zA-Z])/) && passwd.match(/([0-9])/)) // [verified] both letters and numbers
		{
			intScore = (intScore+2)
		}
        // [verified] letters, numbers, and special characters
//		if (passwd.match(/([a-zA-Z0-9].*[!,@,#,$,%,^,&,*,?,_,~])|([!,@,#,$,%,^,&,*,?,_,~].*[a-zA-Z0-9])/))
//		{
//			intScore = (intScore+2)
//		}
		
		return intScore;
}

function checkPwdStrength(password)
{
    document.getElementById('pwdStrengthImg').style.height = '6px'
    document.getElementById('pwdStrengthImg').style.width = '0px';
    document.getElementById('pwdStrengthImg').style.borderTop = '0';
    if(password.length > 5)
    {
        var passwordStrength = parseInt(pwdStrength(password));    
        
        if(passwordStrength > 3)
        {
            width = (passwordStrength / 38 * 91);
            if(width > 93)
            {
                width = 93;
            }
            document.getElementById('pwdStrengthImg').style.width =  width + 'px';
        }
    }
    else if (password.length >= 1)
    {
        document.getElementById('pwdStrengthImg').style.borderTop = '6px solid #E40000';
        document.getElementById('pwdStrengthImg').style.height = '0px'
        document.getElementById('pwdStrengthImg').style.width = '12px';
    } 
}
