#!/usr/bin/env bats

@test "cd command with no arguments" {
    run "./dsh" <<EOF
cd
pwd
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/home/adamterhaerdt/cs283/4-ForkExecdsh2>dsh2>dsh2>cmdloopreturned0"
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "cd command with one argument" {
    run "./dsh" <<EOF
cd bats
pwd
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/home/adamterhaerdt/cs283/4-ForkExec/batsdsh2>dsh2>dsh2>cmdloopreturned0"
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "cd command with invalid argument" {
    run "./dsh" <<EOF
cd invalid_directory
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="cd:Nosuchfileordirectorydsh2>dsh2>cmdloopreturned0"
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "cd command with multiple arguments" {
    run "./dsh" <<EOF
cd testdir1 testdir2
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dsh2>cd:Invalidnumberofargumentsdsh2>cmdloopreturned0"
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "fork and exec command" {
    run "./dsh" <<EOF
ls
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="batsdragon.cdragon.hdshdsh_cli.cdshlib.cdshlib.hmakefilequestions.mdreadme.mddsh2>dsh2>cmdloopreturned0"
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "fork and exec command uname -a" {
    run "./dsh" <<EOF
uname -a
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="LinuxUbuntu6.12.5-orbstack-00287-gf8da5d508983#7SMPTueDec1708:07:20UTC2024x86_64x86_64x86_64GNU/Linuxdsh2>dsh2>cmdloopreturned0"
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "fork and exec command with uname" {
    run "./dsh" <<EOF
uname
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Linuxdsh2>dsh2>cmdloopreturned0"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "fork and exec command with pwd" {
    run "./dsh" <<EOF
pwd
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/home/adamterhaerdt/cs283/4-ForkExecdsh2>dsh2>cmdloopreturned0"
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "invalid command returns error" {
    run "./dsh" <<EOF
unknown
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="execvp:Nosuchfileordirectorydsh2>dsh2>Error:Notavalidcommanddsh2>cmdloopreturned0"
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

