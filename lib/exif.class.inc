<?php

// example usage:
if (false) {
    $exif = new EXIFFormatter("/path/to/image.jpg");
    if (!$exif->valid()) trigger_error("Filename no good", E_USER_ERROR);
    echo $exif->focalLength()."\t".$exif->shutterSpeed()."\t".$exif->fNumber()."\t".$exif->iso()."\n";
}

class EXIFFormatter {
    private static $tool = "/usr/local/bin/exiftool";
    private $filename;

    function __construct($f) {
        $this->filename = false;

        if (!file_exists($f)) {
            trigger_error("File $f does not exist", E_USER_WARNING);
            return;
        }

        if (!assert(is_readable($f))) return;

        $this->filename = $f;
    }

    public function valid() {
        return is_readable($this->filename);
    }

    private function getExifTag($tagName) {
        if ($this->filename === false) trigger_error("Invalid filename", E_USER_ERROR);
        $line = exec(self::$tool." -T -u -".$tagName." ".$this->filename, $unused, $code);

        // Command error
        if ($code !== 0) {
            assert('$code === 0');
            return "";
        }

        // Empty tag
        if ($line == "-") {
            trigger_error("Tag ".$tagName." not found in ".$this->filename, E_USER_WARNING);
            return "";
        }

        return trim($line);
    }

    private static function getDigit($digit) {
        assert("strlen('$digit') == 1");
        if (!assert("is_numeric('$digit')")) return false;
    
        $digit = (int)$digit;
        if (!assert("$digit >= 0 && $digit < 10")) trigger_error("Impossibru!", E_USER_ERROR);
    
        return $digit;
    }

    /*   __                 _   _                  _   _     
     *  / _| ___   ___ __ _| | | | ___ _ __   __ _| |_| |__  
     * | |_ / _ \ / __/ _` | | | |/ _ \ '_ \ / _` | __| '_ \ 
     * |  _| (_) | (_| (_| | | | |  __/ | | | (_| | |_| | | |
     * |_|  \___/ \___\__,_|_| |_|\___|_| |_|\__, |\__|_| |_|
     *                                       |___/           
     *******************************************************/

    private function formatFocalLength($input) {
        $length = strpos($input, " ") !== false
                ? strpos($input, " ")
                : strlen($input);
        $focalLength = substr($input, 0, $length);
        if (!assert("is_numeric('$focalLength') && \$input = '$input'")) return "";
    
        $focalLength = (int)$focalLength;
        if (!assert("$focalLength > 0 && \$input = '$input'")) return "";
    
        return $focalLength."mm";
    }

    private function getFocalLength() {
        return $this->getExifTag("FocalLength");
    }

    public function focalLength() {
        $focalLength = $this->getFocalLength();
        if ($focalLength == "") return "";
        return $this->formatFocalLength($focalLength);
    }
    
    /*      _           _   _                                      _ 
     *  ___| |__  _   _| |_| |_ ___ _ __   ___ _ __   ___  ___  __| |
     * / __| '_ \| | | | __| __/ _ \ '__| / __| '_ \ / _ \/ _ \/ _` |
     * \__ \ | | | |_| | |_| ||  __/ |    \__ \ |_) |  __/  __/ (_| |
     * |___/_| |_|\__,_|\__|\__\___|_|    |___/ .__/ \___|\___|\__,_|
     *                                        |_|                    
     ***************************************************************/

    private function formatShutterSpeed($input) {
        // TODO: non-fractional shutter speeds
        if (!assert("strpos('$input', '/') > 0 && \$input = '$input'")) return "";
        if (!assert("count(explode('/', '$input')) == 2")) return "";
        list($numerator, $denominator) = explode('/', $input);
        if (!assert("is_numeric('$numerator') && is_numeric('$denominator') && \$input = '$input'")) return "";
    
        // Initialize
        $shutterSpeed = "";
        $i = 0;
    
        // Numerator
        while (substr($input, $i, 1) != "/") {
            $digit = self::getDigit(substr($input, $i++, 1));
            if ($digit === false) return "";
    
            $numernumber = array("⁰","¹","²","³","⁴","⁵","⁶","⁷","⁸","⁹");
            $shutterSpeed .= $numernumber[$digit];
        }
    
        // FRACTION SLASH
        $shutterSpeed .= "⁄";
        $i++;
    
        // Denominator
        while ($i < strlen($input)) {
            $digit = self::getDigit(substr($input, $i++, 1));
            if ($digit === false) return "";
            
            $denominumbers = array("₀", "₁", "₂", "₃", "₄", "₅", "₆", "₇", "₈", "₉");
            $shutterSpeed .= $denominumbers[$digit];
        }
    
        return $shutterSpeed;
    }

    private function getShutterSpeed() {
        return $this->getExifTag("ShutterSpeed");
    }

    public function shutterSpeed() {
        $shutterSpeed = $this->getShutterSpeed();
        if ($shutterSpeed == "") return "";
        return $this->formatShutterSpeed($shutterSpeed);
    }
    
    /*   __                         _               
     *  / _|  _ __  _   _ _ __ ___ | |__   ___ _ __ 
     * | |_  | '_ \| | | | '_ ` _ \| '_ \ / _ \ '__|
     * |  _| | | | | |_| | | | | | | |_) |  __/ |   
     * |_|   |_| |_|\__,_|_| |_| |_|_.__/ \___|_|   
     *                                              
     **********************************************/

    private function formatFNumber($input) {
        if (!assert("is_numeric('$input') && \$input = '$input'")) return "";
    
        if (!assert("(int)'$input' > 0 && \$input = '$input'")) return "";
    
        return "ƒ/".$input;
    }
    
    private function getFNumber() {
        return $this->getExifTag("FNumber");
    }

    public function fNumber() {
        $fNumber = $this->getFNumber();
        if ($fNumber == "") return "";
        return $this->formatFNumber($fNumber);
    }


    /*  _           
     * (_)___  ___  
     * | / __|/ _ \ 
     * | \__ \ (_) |
     * |_|___/\___/ 
     *              
     **************/

    private function formatISO($input) {
        if (!assert("is_numeric('$input')")) return "";
    
        if (!assert("(int)'$input' > 0")) return "";
    
        return "ISO ".$input;
    }

    private function getISO() {
        return $this->getExifTag("ISO");
    }

    public function iso() {
        $iso = $this->getISO();
        if ($iso == "") return "";
        return $this->formatISO($iso);
    }
}

?>