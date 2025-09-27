#!/bin/sh

if [ $# -lt 1 ] ; then
    echo "extlib-fetch.sh requires argument" >&2
    exit 1
fi

LIB_NAME="$1"

EXTLIB_BASE_DIR="$(dirname "$(realpath "$0")")"

cd "$EXTLIB_BASE_DIR"

if [ ! -e "config/$LIB_NAME-git.url" ] ; then
    echo "extlib-fetch.sh no URL specified for \"$LIB_NAME\"" >&2
    exit 1
fi

if [ ! -e "config/$LIB_NAME-version.sha" ] ; then
    echo "extlib-fetch.sh no version/SHA specified for \"$LIB_NAME\"" >&2
    exit 1
fi

LIB_GIT_URL="$(head -n 1 "config/$LIB_NAME-git.url")"
LIB_VERSION="$(head -n 1 "config/$LIB_NAME-version.sha")"

#echo "LIB_GIT_URL=$LIB_GIT_URL"
#echo "LIB_VERSION=$LIB_VERSION"

if [ ! -e "$LIB_NAME" ] ; then \
    git clone "$LIB_GIT_URL" "$LIB_NAME" || exit 1
    ( cd "$LIB_NAME" && git branch local "$LIB_VERSION" ) || exit 1
fi
( cd "$LIB_NAME" &&
  git checkout local &&
  git fetch "$LIB_GIT_URL"
) || exit 1

if ( cd "$LIB_NAME" && [ -z "$(git status --porcelain --untracked-files=no)" ] ) ; then
    ( cd "$LIB_NAME" &&
      git reset --hard "$LIB_VERSION" ) || exit 1
else
    ( cd "$LIB_NAME" &&
      git merge "$LIB_VERSION" ) || exit 1
fi

touch .gitignore

if ! grep -q "^$LIB_NAME$" .gitignore ; then
   flock .gitignore -c "echo $LIB_NAME >>.gitignore && cp  .gitignore .gitignore-bak && sort -u .gitignore-bak >.gitignore ; rm .gitignore-bak"
fi
