#!/bin/sh

# Support lttng installed into the user's PATH, but not root's PATH
LTTNG=`which lttng`
LTTNG_SESSIOND=`which lttng-sessiond`

if [ -z "$LTTNG" ]; then
    echo "lttng not found (in PATH), please install lttng"
    exit 1
fi
if [ -z "$LTTNG_SESSIOND" ]; then
    echo "lttng-sessiond not found (in PATH), please install lttng"
    exit 1
fi

ROOT_HOME=`echo ~root`

if [ -z "$(pidof lttng-sessiond)" ]; then
    sudo $LTTNG_SESSIOND --daemonize
fi

output_dir=$HOME/lttng-traces/"$1"
num=1
while [ -d "$output_dir-$num" ]; do num=`expr $num + 1`; done
output_dir="$output_dir-$num"

sudo $LTTNG create -o $output_dir
sudo $LTTNG enable-channel kernel -k --subbuf-size 16M --num-subbuf 8 || exit 2
sudo $LTTNG enable-event -c kernel -k -a || exit 3
sudo $LTTNG enable-channel ust -u || exit 4
sudo $LTTNG enable-event -c ust -u lttng_ust_tracef:* my_provider:* || exit 5
sudo $LTTNG start
$@
sudo $LTTNG stop

sudo chmod a+rx -R $HOME/lttng-traces

echo "Now launch tracecompass."
echo "File / Import. In the wizard select the root directory ${output_dir}"
echo "Make sure to check the Create Experiment checkbox at the bottom."
echo "Then Finish and double click the experiment in the Experiments folder to show the combined event log."
