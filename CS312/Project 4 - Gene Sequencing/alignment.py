import math


def align(
        seq1: str,
        seq2: str,
        match_award=-3,
        indel_penalty=5,
        sub_penalty=1,
        banded_width=-1,
        gap='-'
) -> tuple[float, str | None, str | None]:
    """
        Align seq1 against seq2 using Needleman-Wunsch
        Put seq1 on left (j) and seq2 on top (i)
        => matrix[i][j]
        :param seq1: the first sequence to align; should be on the "left" of the matrix
        :param seq2: the second sequence to align; should be on the "top" of the matrix
        :param match_award: how many points to award a match
        :param indel_penalty: how many points to award a gap in either sequence
        :param sub_penalty: how many points to award a substitution
        :param banded_width: banded_width * 2 + 1 is the width of the banded alignment; -1 indicates full alignment
        :param gap: the character to use to represent gaps in the alignment strings
        :return: alignment cost, alignment 1, alignment 2
    """

    n, m = len(seq1), len(seq2)

    # If the banded algorithm is requested but the sequences are too different
    if banded_width != -1 and abs(n - m) > banded_width:
        return math.inf, None, None

    band_width = 2 * banded_width + 1
    
    # Initialize matrix
    if banded_width == -1:
        matrix = [[0 for _ in range(m + 1)] for _ in range(n + 1)]
    else:
        matrix = [[math.inf for _ in range(band_width + 1)] for _ in range(n + 1)]

    # Initialize the first row and column
    if banded_width == -1:
        for i in range(1, n + 1):
            matrix[i][0] = i * indel_penalty

        for j in range(1, m + 1):
            matrix[0][j] = j * indel_penalty
    else:
        for i in range(1, n + 1):
            matrix[i][0] = i * indel_penalty
        
        for j in range(1, band_width + 1):
            matrix[0][j] = j * indel_penalty
            matrix[0][0] = 0

    # Fill in the matrix based on whether it's banded or not
    for i in range(1, n + 1):
        start, end = 1, m + 1
        
        if banded_width != -1:
            if n-i <= banded_width:
                start = (banded_width + 1) - (n - i)
            else:
                start = 1
            
            if i <= banded_width:
                end = banded_width + i + 1
            else:
                end = band_width + 1

        for j in range(start, end):
            match_or_sub = match_award if seq1[i - 1] == seq2[j - 1] else sub_penalty
                
            
            # Diagonal (match/sub)
            if banded_width == -1 or i <= (1 + banded_width):
                diagonal = matrix[i - 1][j - 1] + match_or_sub
            elif n-i < banded_width:
                k = j
                if n-i < banded_width:
                    k = k - (banded_width - (n - i))
                match_or_sub = (match_award if seq1[i - 1] == seq2[(i - (banded_width + 1)) + k - 1] else sub_penalty)
                diagonal = matrix[i - 1][j-1] + match_or_sub 
            else:
                k = j
                if n-i < banded_width:
                    k = j - (banded_width - (n - i))
                match_or_sub = (match_award if seq1[i - 1] == seq2[(i - (banded_width + 1)) + k - 1] else sub_penalty)
                diagonal = matrix[i - 1][j] + match_or_sub
                
            # Left (deletion)
            if banded_width == -1 or i <= (1 + banded_width):
                left = matrix[i][j - 1] + indel_penalty
            elif n-i < banded_width:
                left = matrix[i][j - 1] + indel_penalty
            else:
                if(j == 1):
                    left = math.inf + indel_penalty
                else:
                    left = matrix[i][j - 1] + indel_penalty
            
            # Top (insertion)
            if banded_width == -1 or i <= (1 + banded_width):
                top = matrix[i - 1][j] + indel_penalty
            elif n-i < banded_width:
                top = matrix[i - 1][j] + indel_penalty
            else:
                if(j == band_width):
                    top = math.inf + indel_penalty
                else:
                    top = matrix[i - 1][j + 1] + indel_penalty

            # Take minimum cost
            matrix[i][j] = min(diagonal, top, left)
            

    # DEBUG: Print matrix for testing
    # print("\nMatrix after filling:")
    # for row in matrix:
    #     print(row)
    # print()

    # Traceback to get aligned sequences
    align1, align2 = '', ''

    if banded_width == -1:
        i, j = n, m
        while i > 0 or j > 0:
            current = matrix[i][j]
            match_or_sub = match_award if seq1[i - 1] == seq2[j - 1] else sub_penalty

            # Diagonal (preferable)
            if i > 0 and j > 0:
                if current == (matrix[i - 1][j - 1] + match_or_sub):
                    align1 = seq1[i - 1] + align1
                    align2 = seq2[j - 1] + align2
                    i -= 1
                    j -= 1
                    continue
            
            # Left (deletion in seq2)
            if j > 0:
                if current == matrix[i][j - 1] + indel_penalty:
                    align1 = gap + align1
                    align2 = seq2[j - 1] + align2
                    j -= 1
                    continue

            # Top (insertion in seq1)
            if i > 0:
                if current == matrix[i - 1][j] + indel_penalty:
                    align1 = seq1[i - 1] + align1
                    align2 = gap + align2
                    i -= 1
                    continue
    else:
        i, j, l = n, band_width, n
        while i > 0 or j > 0:
            current = matrix[i][j]
            
            # Diagonal (preferable)
            if i > 0:
                diagonal = matrix[i-1][j]
                if n - i < banded_width:
                    diagonal = matrix[i-1][j-1]
                elif i <= banded_width + 1 and j > 0:
                    diagonal = matrix[i-1][j-1]
                    
                k = j
                if n-i < banded_width:
                    k = k - (banded_width - (n - i))
                match_or_sub = (match_award if seq1[i - 1] == seq2[l - 1] else sub_penalty)
                if current == diagonal + match_or_sub:
                    align1 = seq1[i - 1] + align1
                    align2 = seq2[l - 1] + align2
                    i -= 1
                    l -= 1
                    if n-i < banded_width + 1 or i <= banded_width:
                        j -= 1
                    continue
            
            # Left (deletion in seq2)
            if j > 0:
                left = matrix[i][j - 1]
                if current == left + indel_penalty:
                    align1 = gap + align1
                    align2 = seq2[l - 1] + align2
                    j -= 1
                    l -= 1
                    continue
            
            # Top (insertion in seq1)
            if i > 0:
                if j == band_width:
                    continue
                top = matrix[i - 1][j + 1]
                if i <= banded_width + 1 or n-i < banded_width:
                    top = matrix[i-1][j]
                if current == top + indel_penalty:
                    align1 = seq1[i - 1] + align1
                    align2 = gap + align2
                    i -= 1
                    l -= 1
                    continue
                
            break
            
    if banded_width == -1:
        return matrix[n][m], align1, align2
    else:
        return matrix[n][band_width], align1, align2