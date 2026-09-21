#!/bin/bash

# Release: run unit tests, verify CMake project version is newer than the
# latest git tag, create vX.Y.Z, then push master and the tag to origin
# (Gitee) and github.

set -euo pipefail

usage()
{
    echo "usage: $0"
    echo ""
    echo "Steps:"
    echo "  1. Build and run unit tests (C++11, same as the release library)."
    echo "  2. Require CMakeLists.txt project VERSION > latest v* tag."
    echo "  3. Create tag v<VERSION> on the current master commit."
    echo "  4. Push master and the new tag to remotes origin and github."
    echo ""
    echo "Must be run on a clean master branch."
    exit 0
}

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    usage
fi

script_dir=$(cd "$(dirname "$0")" && pwd)
repo_root=$(cd "${script_dir}/.." && pwd)
cd "${repo_root}"

die()
{
    echo "error: $*" >&2
    exit 1
}

# true if $1 > $2 (dot-separated numeric versions, e.g. 1.8.7 vs 1.8.6)
version_gt()
{
    local IFS=.
    local -a a=($1) b=($2)
    local i n=${#a[@]}
    if [ ${#b[@]} -gt "$n" ]; then
        n=${#b[@]}
    fi
    for ((i = 0; i < n; i++)); do
        local x=${a[i]:-0}
        local y=${b[i]:-0}
        if ((10#$x > 10#$y)); then
            return 0
        fi
        if ((10#$x < 10#$y)); then
            return 1
        fi
    done
    return 1
}

echo "==> repo: ${repo_root}"

# ---------------------------------------------------------------------------
# Preconditions
# ---------------------------------------------------------------------------
[ -d .git ] || die "not a git repository: ${repo_root}"
[ -f CMakeLists.txt ] || die "CMakeLists.txt not found"

branch=$(git rev-parse --abbrev-ref HEAD)
[ "${branch}" = "master" ] || die "current branch is '${branch}', expected master"

if [ -n "$(git status --porcelain)" ]; then
    die "working tree is not clean; commit or stash changes before release"
fi

git remote get-url origin >/dev/null 2>&1 || die "remote 'origin' is not configured"
git remote get-url github >/dev/null 2>&1 || die "remote 'github' is not configured"

# ---------------------------------------------------------------------------
# 1. Unit tests (C++11, matching script/build.sh release)
# ---------------------------------------------------------------------------
echo "==> [1/4] running unit tests"
UT_CXX11=ON "${script_dir}/build.sh" test
echo "==> unit tests passed"

# ---------------------------------------------------------------------------
# 2. CMake project version must be greater than the latest tag
# ---------------------------------------------------------------------------
echo "==> [2/4] checking version against latest tag"

git fetch origin --tags
git fetch github --tags

cmake_version=$(sed -nE 's/^project\([^)]*VERSION[ \t]+([0-9]+(\.[0-9]+)*)[ \t].*/\1/p' CMakeLists.txt | head -n 1)
[ -n "${cmake_version}" ] || die "could not parse project VERSION from CMakeLists.txt"

latest_tag=$(git tag -l 'v[0-9]*' --sort=-v:refname | head -n 1)
if [ -z "${latest_tag}" ]; then
    latest_version="0.0.0"
    echo "    no existing v* tag; treating latest as ${latest_version}"
else
    latest_version=${latest_tag#v}
    echo "    latest tag: ${latest_tag} (version ${latest_version})"
fi

echo "    CMakeLists.txt VERSION: ${cmake_version}"

if [ "${cmake_version}" = "${latest_version}" ] || ! version_gt "${cmake_version}" "${latest_version}"; then
    die "CMakeLists.txt VERSION ${cmake_version} must be greater than latest tag ${latest_tag:-none}"
fi

new_tag="v${cmake_version}"
if git rev-parse -q --verify "refs/tags/${new_tag}" >/dev/null; then
    die "tag ${new_tag} already exists locally"
fi

echo "==> version check passed (${cmake_version} > ${latest_version})"

# ---------------------------------------------------------------------------
# 3. Create tag from CMakeLists.txt version
# ---------------------------------------------------------------------------
echo "==> [3/4] creating tag ${new_tag}"
git tag -a "${new_tag}" -m "Release ${new_tag}"
echo "==> tagged $(git rev-parse --short HEAD) as ${new_tag}"

# ---------------------------------------------------------------------------
# 4. Push master and the new tag to origin and github
# ---------------------------------------------------------------------------
echo "==> [4/4] pushing master and ${new_tag} to origin and github"
git push origin master
git push origin "${new_tag}"
git push github master
git push github "${new_tag}"

echo "==> release ${new_tag} done"
