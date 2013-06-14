/* libmpdclient
 (c)2003-2006 by Warren Dukes (warren.dukes@gmail.com)
 This project's homepage is: http://www.musicpd.org

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 - Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 - Neither the name of the Music Player Daemon nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <mpd/client.h>
#include <mpd/status.h>
#include <mpd/entity.h>
#include <mpd/search.h>
#include <mpd/tag.h>
#include <mpd/playlist.h>
#include <mpd/queue.h>
#include <mpd/directory.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "RadioManager.h"

using namespace std;

static void print_tag( const struct mpd_song *song, enum mpd_tag_type type, const char *label) {
	unsigned i = 0;
	const char *value;

//	while ((value = mpd_song_get_tag( song, type, i++)) != NULL)
	value = mpd_song_get_tag( song, type, i);
	if (value != NULL)
		printf( "%s: %s\n", label, value);
}

int umain( int argc, char ** argv) {
	RadioManager::instance()->changeVolume(10);
	struct mpd_connection *_mpdConnect;
	//return 0;
	_mpdConnect = mpd_connection_new( NULL, 0, 30000);

	if (mpd_connection_get_error( _mpdConnect) != MPD_ERROR_SUCCESS) {
		fprintf( stderr, "%s\n", mpd_connection_get_error_message( _mpdConnect));
		mpd_connection_free( _mpdConnect);
		return -1;
	}

	{
		int i;
		for (i = 0; i < 3; i++) {
			printf( "version[%i]: %i\n", i, mpd_connection_get_server_version( _mpdConnect)[i]);
		}
	}

	//mpd_command_list_begin( _mpdConnect, true);
	mpd_send_list_queue_meta( _mpdConnect);
	//mpd_command_list_end( _mpdConnect);

	mpd_song* oneRadio;
	while ((oneRadio = mpd_recv_song( _mpdConnect)) != NULL) {
		printf( "uri: %s\n", mpd_song_get_uri( oneRadio));
		print_tag( oneRadio, MPD_TAG_ARTIST, "artist");
		print_tag( oneRadio, MPD_TAG_ALBUM, "album");
		print_tag( oneRadio, MPD_TAG_TITLE, "title");
		print_tag( oneRadio, MPD_TAG_TRACK, "track");
		print_tag( oneRadio, MPD_TAG_NAME, "name");
		print_tag( oneRadio, MPD_TAG_DATE, "date");

		if (mpd_song_get_duration( oneRadio) > 0) {
			printf( "time: %u\n", mpd_song_get_duration( oneRadio));
		}

		printf( "pos: %u\n", mpd_song_get_pos( oneRadio));

		mpd_song_free( oneRadio);
	}

	if (argc == 1) {
		printf( "toto ca marche bordel!");
		struct mpd_status * status;
		struct mpd_song *song;
		const struct mpd_audio_format *audio_format;

		bool result = mpd_command_list_begin( _mpdConnect, true);
		result = mpd_send_status( _mpdConnect);
		result = mpd_send_current_song( _mpdConnect);
		result = mpd_command_list_end( _mpdConnect);

		status = mpd_recv_status( _mpdConnect);
		if (status == NULL) {
			std::cout << "mpd_recv_status return false with message : "
					<< mpd_connection_get_error_message( _mpdConnect) << std::endl;
			fprintf( stderr, "%s\n", mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}

		printf( "volume: %i\n", mpd_status_get_volume( status));
		printf( "repeat: %i\n", mpd_status_get_repeat( status));
		printf( "queue version: %u\n", mpd_status_get_queue_version( status));
		printf( "queue length: %i\n", mpd_status_get_queue_length( status));
		if (mpd_status_get_error( status) != NULL)
			printf( "error: %s\n", mpd_status_get_error( status));

		if (mpd_status_get_state( status) == MPD_STATE_PLAY || mpd_status_get_state( status) == MPD_STATE_PAUSE) {
			printf( "song: %i\n", mpd_status_get_song_pos( status));
			printf( "elaspedTime: %i\n", mpd_status_get_elapsed_time( status));
			printf( "elasped_ms: %u\n", mpd_status_get_elapsed_ms( status));
			printf( "totalTime: %i\n", mpd_status_get_total_time( status));
			printf( "bitRate: %i\n", mpd_status_get_kbit_rate( status));
		}

		audio_format = mpd_status_get_audio_format( status);
		if (audio_format != NULL) {
			printf( "sampleRate: %i\n", audio_format->sample_rate);
			printf( "bits: %i\n", audio_format->bits);
			printf( "channels: %i\n", audio_format->channels);
		}

		if (mpd_connection_get_error( _mpdConnect) != MPD_ERROR_SUCCESS) {
			fprintf( stderr, "%s\n", mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}

		mpd_response_next( _mpdConnect);

		while ((song = mpd_recv_song( _mpdConnect)) != NULL) {
			printf( "uri: %s\n", mpd_song_get_uri( song));
			print_tag( song, MPD_TAG_ARTIST, "artist");
			print_tag( song, MPD_TAG_ALBUM, "album");
			print_tag( song, MPD_TAG_TITLE, "title");
			print_tag( song, MPD_TAG_TRACK, "track");
			print_tag( song, MPD_TAG_NAME, "name");
			print_tag( song, MPD_TAG_DATE, "date");

			if (mpd_song_get_duration( song) > 0) {
				printf( "time: %u\n", mpd_song_get_duration( song));
			}

			printf( "pos: %u\n", mpd_song_get_pos( song));

			mpd_song_free( song);
		}

		if (mpd_connection_get_error( _mpdConnect) != MPD_ERROR_SUCCESS) {
			fprintf( stderr, "%s\n", mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}

		if (!mpd_response_finish( _mpdConnect)) {
			fprintf( stderr, "%s\n", mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}

		mpd_status_free( status);
	} else if (argc == 3 && strcmp( argv[1], "lsinfo") == 0) {
		struct mpd_entity * entity;
		std::cout << "tu passe par la?" << std::endl;
		if (!mpd_send_list_meta( _mpdConnect, argv[2])) {
			fprintf( stderr, "erreur de mpd_send_list_meta : %s\n", mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}

		while ((entity = mpd_recv_entity( _mpdConnect)) != NULL) {
			const struct mpd_song *song;
			const struct mpd_directory *dir;
			const struct mpd_playlist *pl;

			switch (mpd_entity_get_type( entity)) {
			case MPD_ENTITY_TYPE_UNKNOWN:
				break;

			case MPD_ENTITY_TYPE_SONG:
				song = mpd_entity_get_song( entity);
				printf( "uri: %s\n", mpd_song_get_uri( song));
				print_tag( song, MPD_TAG_ARTIST, "artist");
				print_tag( song, MPD_TAG_ALBUM, "album");
				print_tag( song, MPD_TAG_TITLE, "title");
				print_tag( song, MPD_TAG_TRACK, "track");
				break;

			case MPD_ENTITY_TYPE_DIRECTORY:
				dir = mpd_entity_get_directory( entity);
				printf( "directory: %s\n", mpd_directory_get_path( dir));
				break;

			case MPD_ENTITY_TYPE_PLAYLIST:
				pl = mpd_entity_get_playlist( entity);
				printf( "playlist: %s\n", mpd_playlist_get_path( pl));
				break;
			}

			mpd_entity_free( entity);
		}

		if (mpd_connection_get_error( _mpdConnect) != MPD_ERROR_SUCCESS) {
			fprintf( stderr, "erreur dans mpd_connection_get_error : %s\n",
					mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}

		if (!mpd_response_finish( _mpdConnect)) {
			fprintf( stderr, "erreur de mpd_response_finish : %s\n", mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}
	} else if (argc == 2 && strcmp( argv[1], "artists") == 0) {
		struct mpd_pair *pair;

		if (!mpd_search_db_tags( _mpdConnect, MPD_TAG_ARTIST) || !mpd_search_commit( _mpdConnect)) {
			fprintf( stderr, "%s\n", mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}

		while ((pair = mpd_recv_pair_tag( _mpdConnect, MPD_TAG_ARTIST)) != NULL) {
			printf( "%s\n", pair->value);
			mpd_return_pair( _mpdConnect, pair);
		}

		if (mpd_connection_get_error( _mpdConnect) != MPD_ERROR_SUCCESS) {
			fprintf( stderr, "%s\n", mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}

		if (!mpd_response_finish( _mpdConnect)) {
			fprintf( stderr, "%s\n", mpd_connection_get_error_message( _mpdConnect));
			mpd_connection_free( _mpdConnect);
			return -1;
		}
	}

	mpd_connection_free( _mpdConnect);

	return 0;
}
