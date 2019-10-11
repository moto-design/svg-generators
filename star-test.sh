#!/usr/bin/env bash

stars="
	5-2
	6-2
	7-2
	7-3
	8-2
	8-3
	9-2
	9-3
	9-4
	10-2
	10-3
	10-4
"

set -ex

for star in ${stars}; do
	./star-generator --points=${star%-*} --density=${star#*-} ${extra} -o /tmp/star-${star}.svg
done
