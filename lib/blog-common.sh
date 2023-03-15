# settings:
export DB=$BLOGROOT/data/blog.db
export TZ="America/Los_Angeles"

# alright, stop touching stuff.

# make sure php isn't going to throw an error that winds up creating a lot of strange looking directories
if [ "x$(date_default_timezone_set('America/Los_Angeles'); php -r "date('Y');" 2>&1)" != "x" ]; then
   php -r "date_default_timezone_set('America/Los_Angeles'); date('Y');"
   exit
fi

# check for consistent state:
if [ ! -f $DB ]; then
	echo "SH : WARNING: no database found, creating new"
	sqlite3 $DB < $BLOGROOT/data/schema.sql
fi

# library:

function get-content {
	cat $1/post.html | sed -e "s/\'/\'\'/g"
}

function get-location {
	location=$(cd $1; pwd)
	echo $(basename $(dirname $location))/$(basename $location)
}

# determine if the path points to a valid post directory based on its contents
function post-valid {
	if [ ! -d $1  ]; then
		echo "SH : ERROR: $1 is not a valid directory!"
		exit
	fi
	if [ ! -f $1/post.html ]; then
		echo "SH : ERROR: $1/post.html missing!"
		exit 1
	fi
	if [ ! -f $1/timestamp.txt ]; then
		echo "SH : WARN: no timestamp, creating"
		date > $1/timestamp.txt
	fi
}

# 1 argument: location
function post-make-page {
	location=$1
	
	echo "PHP: generating post/$location.html"
	php -d log_errors=On -d display_errors=Off \
		$BLOGROOT/bin/make-post.php $location > $BLOGROOT/post/$location.html || exit
}

# 0 arguments
function post-make-index {
	echo "PHP: generating index.html"
	php -d log_errors=On -d display_errors=Off \
		$BLOGROOT/bin/make-index.php > $BLOGROOT/index.html || exit
}

# 0 arguments
function post-make-feed {
	echo "PHP: generating feed.atom"
	php -d log_errors=On -d display_errors=Off \
		$BLOGROOT/bin/make-atom.php > $BLOGROOT/feed.atom || exit
}
