1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines when a command's output is fully received from the server by checking if the message ends with the `RDSH_EOF_CHAR` character. A technique that can be used to handle partial reads or ensure complete message transmission is to print the number of bytes received minus one (to exclude the EOF character) and then print the message.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

There are a few ways that a networked shell protocol should define and detect the beginning and end of a command. The first is to use a delimiter such as the null terminator or the EOF character. Another way you could define and detect the beginning and end of a command is to send the length of the message in the protocol. The challeneges when message coundaries are not handled correctly is that it could potentially lead to commands being incomplete or partial outputs. The next is that there could be buffer overflow potential. Without properly handling the legnth of the message we might have buffer overflow errors which could be a security vulenerability.

3. Describe the general differences between stateful and stateless protocols.

The difference between stateful and stateless protocols is that stateless protocols needs to contain all the information within the request itself needed for processing. In a stateful protocol the client's session is maintained across multiple requests. The server keeps track of the client's state and uses that information for other requests the client sends. In a stateful procol, it typically uses more memory as you need to maintain the clients state, where in the serverless protocol it uses less memory as no state persists.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

While UDP is considered unreliable, there are many advantages of why we would use UDP. The first is that it is more simple and efficient than TCP. UDP has easier state management compared to TCP. There is no acknowledgement waiting for sending data meaining that there would be lower latency. After some research, I realized that UDP is also good for gaming because it is lower latency than TCP.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The Socket API is a fundamental and widely used network programming interface that is provided by the operating system to enable applications to use network communications.