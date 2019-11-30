bayan
Files duplicates search utility.

Command line options:
  -h [ --help ]                produce help message
  -I [ --include-dirs ] arg    include directories for scan
  -E [ --exclude-dirs ] arg    exclude directories for scan
  -M [ --file-mask ] arg       files names masks(as regexp) to scan
  -R [ --recursive ] arg (=0)  0- do not scan nested directories, 1- recursive scan
  -B [ --min-size ] arg (=1)   min file size(bytes) to scan
  -S [ --block-size ] arg (=1) scan block size(bytes)
  -C [ --conv ] arg (=md5)     convolution algorithm