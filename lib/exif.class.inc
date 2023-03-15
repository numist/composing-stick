<?php

// example usage:
if (true) {
    $exif = new EXIFFormatter("/Users/numist/Desktop/IMG_8972.jpg");
    if (!$exif->valid()) trigger_error("EXIFFormatter won't work, bad file or could not find exiftool", E_USER_ERROR);
    echo $exif->focalLength()."\t".$exif->shutterSpeed()."\t".$exif->fNumber()."\t".$exif->iso()."\n";
}

class EXIFFormatter {
    private $data;

    function __construct($f) {
        if (!function_exists("exif_read_data")) die("Missing function exif_read_data");
        $this->data = exif_read_data($f);
    }

    public function valid() {
        return $this->data !== false;
    }

    private function getExifTag($tagName, $computed=false) {
        if (!$computed) {
            if (!array_key_exists($tagName, $this->data)) return "";
            return $this->data[$tagName];
        } else {
            if (!array_key_exists($tagName, $this->data["COMPUTED"])) return "";
            return $this->data["COMPUTED"][$tagName];
        }
    }

    private static function getDigit($digit) {
        if (strlen($digit) !== 1) {
            assert("strlen('$digit') === 1");
            return false;
        }

        if (!is_numeric($digit)) {
            assert("is_numeric('$digit')");
            return false;
        }

        return (int)$digit;
    }

    /*   __                 _   _                  _   _     
     *  / _| ___   ___ __ _| | | | ___ _ __   __ _| |_| |__  
     * | |_ / _ \ / __/ _` | | | |/ _ \ '_ \ / _` | __| '_ \ 
     * |  _| (_) | (_| (_| | | | |  __/ | | | (_| | |_| | | |
     * |_|  \___/ \___\__,_|_| |_|\___|_| |_|\__, |\__|_| |_|
     *                                       |___/           
     *******************************************************/

    private function formatFocalLength($input) {
        $input = (int)$input; // TODO: actually math, format is generally ##/1

        if ($input <= 0) {
            assert("$input > 0");
            return "";
        }
    
        return $input."mm";
    }

    private function getFocalLength() {
        return $this->getExifTag("FocalLength");
    }

    public function hasFocalLength() {
        return $this->getFocalLength() != "";
    }

    public function focalLength() {
        if (!$this->hasFocalLength()) return "";
        return $this->formatFocalLength($this->getFocalLength());
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
        if (strpos($input, '/') == 0) {
            assert("strpos('$input', '/') > 0 && \$input = '$input'");
            return "";
        }

        if (count(explode('/', $input)) != 2) {
            assert("count(explode('/', '$input')) == 2");
            return "";
        }

        list($numerator, $denominator) = explode('/', $input);
        if (!is_numeric($numerator) || !is_numeric($denominator)) {
            assert("is_numeric('$numerator') && is_numeric('$denominator') && \$input = '$input'");
            return "";
        }
    
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
        return $this->getExifTag("ExposureTime");
    }

    public function hasShutterSpeed() {
        return $this->getShutterSpeed() != "";
    }

    public function shutterSpeed() {
        if (!$this->hasShutterSpeed()) return "";
        return $this->formatShutterSpeed($this->getShutterSpeed());
    }
    
    /*   __                         _               
     *  / _|  _ __  _   _ _ __ ___ | |__   ___ _ __ 
     * | |_  | '_ \| | | | '_ ` _ \| '_ \ / _ \ '__|
     * |  _| | | | | |_| | | | | | | |_) |  __/ |   
     * |_|   |_| |_|\__,_|_| |_| |_|_.__/ \___|_|   
     *                                              
     **********************************************/

    private function formatFNumber($input) {
        $leader = "f/"; // input in format f/16; trim "f/" leader
        if (strpos($input, $leader) === 0) {
            $input = substr($input, strpos($input, $leader) + strlen($leader));
        }

        if (!is_numeric($input)) {
            assert("is_numeric('$input') && \$input = '$input'");
            return "";
        }
    
        if ((int)$input <= 0) {
            assert("(int)'$input' > 0 && \$input = '$input'");
            return "";
        }
    
        return "ƒ/".$input;
    }
    
    private function getFNumber() {
        return $this->getExifTag("ApertureFNumber", true);
    }

    public function hasFNumber() {
        return $this->getFNumber() != "";
    }

    public function fNumber() {
        if (!$this->hasFNumber()) return "";
        return $this->formatFNumber($this->getFNumber());
    }


    /*  _           
     * (_)___  ___  
     * | / __|/ _ \ 
     * | \__ \ (_) |
     * |_|___/\___/ 
     *              
     **************/

    private function formatISO($input) {
        if (!is_int($input)) {
            assert("is_int('$input')");
            return "";
        }
    
        return "ISO ".$input;
    }

    private function getISO() {
        return $this->getExifTag("ISOSpeedRatings");
    }

    public function hasISO() {
        return $this->getISO() != "";
    }

    public function iso() {
        if (!$this->hasISO()) return "";
        return $this->formatISO($this->getISO());
    }
}

?>