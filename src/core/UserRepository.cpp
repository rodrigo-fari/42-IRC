#include "../../inc/core/UserRepository.hpp"

User* UserRepository::findUserByUsername(const std::string& username) {
    std::map<std::string, User>::iterator it = usersByUsername.find(username);
    if (it != usersByUsername.end()) {
        return &(it->second);
    }
    return NULL;
}

User* UserRepository::findUserByFileDescriptor(int fd) {
    std::map<int, User>::iterator it = usersByFileDescriptor.find(fd);
    if (it != usersByFileDescriptor.end()) {
        return &(it->second);
    }
    return NULL;
}
